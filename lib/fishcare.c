#include "fishcare.h"

FishClient client;
FeedingList *fl = NULL;

void fishcare_mode() {
    printf("Iniciando FishCare\n");
    gpio_put(GREEN_LED, 0);

    // Mensagem inicial
    oled_clear();
    oled_print(3, "Modo FishCare");

    find_next_feed(fl);
    sleep_ms(1000);
    
    //Checa se não há horários salvos
    if(client.connected != 1){
        // Exibe IP e aguarda conexão do app
        wait_for_app_connection();
    }

    // Exibe informações no display
    oled_clear();

    // Inicia as tarefas em threads
    multicore_launch_core1(feeding_check_task);  // Checagem de tempo no Core 1

    // Atualização do display e checagem de alimentação no Core 0
    update_display_task();
}

//Função para girar o motor e alimentar o peixe
void handle_feeding() {
    if (client.remainingFeedings > 0) {
        oled_clear();
        oled_print(4, "Hora de comer!");
        sleep_ms(1500);

        printf("Executando alimentação...\n");
        motor_move(34, 4);// Passos e intervalo
        client.remainingFeedings--;

        //Seta ultima alimentacao
        uint8_t current_hour, current_minute;
        get_current_time(&current_hour, &current_minute);

        client.last_feed.hour = current_hour;
        client.last_feed.minute = current_minute;

        printf("Alimentações restantes: %d\n", client.remainingFeedings);
    } else {
        printf("Nenhuma alimentação restante para hoje.\n");
    }
}

// Função para adicionar um horário na lista, ordenado do menor para o maior
void add_feed(FeedingList **lista, uint8_t hour, uint8_t minute) {
    FeedingList *novo = malloc(sizeof(FeedingList));
    novo->time.hour = hour;
    novo->time.minute = minute;
    novo->prox = NULL;

    printf("\nAdicionando horário: %02d:%02d na lista\n", hour, minute);

    // Se a lista estiver vazia, o novo nó será o primeiro
    if (*lista == NULL) {
        *lista = novo;
        add_feeding_time(hour,minute);
        return;
    }

    FeedingList *aux = *lista, *anterior = NULL;

    // Insere o novo horário na posição correta (ordem crescente)
    while (aux != NULL) {
        int comparison = compare_time(hour, minute, aux->time.hour, aux->time.minute);
        if (comparison < 0) { // Novo horário deve vir antes do atual
            break;
        }
        anterior = aux;
        aux = aux->prox;
    }

    // Caso especial: inserir no início
    if (anterior == NULL) {
        novo->prox = *lista;
        *lista = novo;
    } else { // Inserir no meio ou final
        anterior->prox = novo;
        novo->prox = aux;
    }

    add_feeding_time(hour,minute);
}

// Função para remover um horário da lista
bool remove_feed(FeedingList **lista, uint8_t hour, uint8_t minute) {
    FeedingList *aux = *lista, *anterior = NULL;

    while (aux != NULL) {
        if (aux->time.hour == hour && aux->time.minute == minute) {
            // Remove o nó da lista
            if (anterior == NULL) {
                *lista = aux->prox;  // Remove do início
            } else {
                anterior->prox = aux->prox;  // Remove do meio ou final
            }
            free(aux);
            remove_feeding_time(hour,minute);
            return true;
        }
        anterior = aux;
        aux = aux->prox;
    }
    return false;
}

// Função para imprimir a lista de horários
void print_feed_list(FeedingList *lista) {
    FeedingList *aux = lista;
    printf("Lista de horários: ");
    while (aux != NULL) {
        printf("%02d:%02d -> ", aux->time.hour, aux->time.minute);
        aux = aux->prox;
    }
    printf("NULL\n");
}
void update_display_oled(){

    char temp_msg[20];

    // Atualiza horário atual
    uint8_t current_hour, current_minute;
    get_current_time(&current_hour, &current_minute);

    sprintf(temp_msg, "conectado! %02d:%02d", current_hour, current_minute);
    oled_print(0, temp_msg);

    // Atualiza temperatura no display
    float current_temperature = get_temperatura();
    sprintf(temp_msg, "temp-------%.1fC", current_temperature);
    oled_print(2, temp_msg);

    // Exibe próximo horário de alimentação
    sprintf(temp_msg, "proximo----%02d:%02d", client.next_feed.hour, client.next_feed.minute);
    if(client.next_feed.hour > 24){
        strcpy(temp_msg, "proximo-----none");
    }
    oled_print(3, temp_msg);

    //ip
    strcpy(temp_msg, wifi_ip());
    oled_print(5, temp_msg);

    // Exibe a quantidade restante
    sprintf(temp_msg, "Remain %d drops", client.remainingFeedings);
    oled_print(7, temp_msg);
}

void feeding_check_task() {
   
    while (1) {
        
        uint8_t current_hour, current_minute;
        get_current_time(&current_hour, &current_minute);

        print_feed_list(fl);

        // Itera pela lista de horários
        FeedingList *aux = fl;
        while (aux != NULL) {
            if (current_hour == aux->time.hour && current_minute == aux->time.minute) {
                if (client.last_feed.hour == aux->time.hour && aux->time.minute == client.last_feed.minute){
                    printf("Já alimentado!");
                    break;
                }

                printf("Hora de alimentar os peixes: %02d:%02d\n", aux->time.hour, aux->time.minute);
                handle_feeding();

                client.last_feed = aux->time;
                if(aux->prox == NULL) client.next_feed = fl->time;
                else client.next_feed = aux->prox->time;

                save_data();
                break;
            }
            aux = aux->prox;
        }

        sleep_ms(30000);  // Verifica a cada 30 segundos
    }
}

void update_display_task() {
    while (true) {
        datetime_t t;
        rtc_get_datetime(&t);
        printf("Hora atual: %02d:%02d:%02d\n", t.hour, t.min, t.sec);
        
        update_display_oled();
        sleep_ms(10000);  // Atualiza a cada 10 segundos
    }
}

void update_feed_list(){ //Atualiza a lista encadeada com a lista de alimentacoes
    for(int i = 0; i < client.scheduleCount; i++){
        printf("\nAdicionando horário: %02d:%02d na lista\n", client.feedingSchedule[i].hour, client.feedingSchedule[i].minute);

        add_feed(&fl,client.feedingSchedule[i].hour,client.feedingSchedule[i].minute);
    }
}

void find_next_feed(FeedingList *lista) {
    uint8_t current_hour, current_minute;
    get_current_time(&current_hour, &current_minute); // Obtém a hora atual

    FeedingList *aux = lista;

    while (aux != NULL) {
        // Se o horário for maior que o atual, ele é o próximo
        if (compare_time(current_hour, current_minute, aux->time.hour, aux->time.minute) == -1) {
            client.next_feed = aux->time;
            return;
        }
        aux = aux->prox;
    }

    // Se nenhum horário futuro for encontrado, pega o primeiro da lista (dia seguinte)
    if (lista != NULL) {
        client.next_feed = lista->time;
    }

    return;
}
