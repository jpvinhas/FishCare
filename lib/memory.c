#include "memory.h"

// Define um valor padrão para os dados caso a memória esteja vazia
void set_default_values() {
    client.remainingFeedings = 16;
    client.scheduleCount = 0;
    client.connected = 0;
    client.next_feed.hour = 99;
    client.next_feed.hour = 99;
    client.last_feed.hour = 99;
    client.last_feed.minute = 99;
}

// Salva os dados na memória flash
void save_data() {
    uint8_t buffer[FLASH_PAGE_SIZE]; // Buffer temporário do tamanho da página de flash

    // Limpa o buffer antes de copiar os dados
    memset(buffer, 0xFF, FLASH_PAGE_SIZE);
    memcpy(buffer, &client, sizeof(FishClient));

    uint32_t ints = save_and_disable_interrupts(); // Desativa interrupções para segurança
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE); // Apaga setor antes de escrever
    flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
    restore_interrupts(ints); // Restaura interrupções

    printf("Dados salvos na memória flash!\n");
}

//  Carrega os dados da memória flash
void load_data() {
   
    const uint8_t *flash_memory = ((const uint8_t *)XIP_BASE) + FLASH_TARGET_OFFSET;
    memcpy(&client, flash_memory, sizeof(FishClient));

    // Verifica se os dados são válidos
    if (client.connected != 1) {
        printf(" Nenhum dado salvo. Definindo valores padrão...\n");
        set_default_values();
        save_data();
    } else {
        printf("Dados carregados com sucesso!\n");
    }

    printf("\nconected: %d, next: %d \n, foods: %d",client.connected,client.feedingSchedule[0].hour,client.scheduleCount);
    sleep_ms(500);
}

// Adiciona um novo horário de alimentação
void add_feeding_time(uint8_t hour, uint8_t minute) {

    for (int i = 0; i < client.scheduleCount; i++) {
        if (client.feedingSchedule[i].hour == hour && client.feedingSchedule[i].minute == minute) {
            printf("Horário de alimentação já cadastrado: %02d:%02d\n", hour, minute);
            return;
        }
    }

    client.feedingSchedule[client.scheduleCount].hour = hour;
    client.feedingSchedule[client.scheduleCount].minute = minute;
    client.scheduleCount++;
    printf("Adicionando horário: %02d:%02d\n na memoria", hour, minute);

    find_next_feed(fl);

    printf("next: %d", client.next_feed.hour);

    save_data();  // Salva a nova configuração
    printf("Novo horário de alimentação adicionado: %02d:%02d\n", hour, minute);
}

//  Remove um horário de alimentação da lista
void remove_feeding_time(uint8_t hour, uint8_t minute) {
    if (client.scheduleCount == 0) {
        printf("Nenhum horário para remover!\n");
        return;
    }

    for (int i = 0; i < client.scheduleCount; i++) {
        if (client.feedingSchedule[i].hour == hour && client.feedingSchedule[i].minute == minute) {
            // Move os elementos para preencher a lacuna
            for (int j = i; j < client.scheduleCount - 1; j++) {
                client.feedingSchedule[j] = client.feedingSchedule[j + 1];
            }
            client.scheduleCount--;

            save_data();  // Salva a nova configuração
            printf("Horário de alimentação removido da memoria: %02d:%02d\n", hour, minute);
            return;
        }
    }

    printf("Horário não encontrado!\n");
}

// Inicializa os dados na inicialização do sistema
void init_client() {
    load_data();

    update_feed_list();
}

void reset_client() {
    // Limpa toda a estrutura (opcional, mas útil para garantir que não haja lixo)
    memset(&client, 0, sizeof(client));

    // Define os valores padrão para o client
    set_default_values();

    // Salva os novos valores na flash
    save_data();

    //remove horários da lista
    fl->prox = NULL;
}