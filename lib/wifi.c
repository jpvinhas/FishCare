#include "wifi.h"

int connection_count = 0;    // Inicialização do contador de conexões

//Inicia servidor
void server_init() {
    printf("Iniciando Servidor...\n");
    struct tcp_pcb *pcb = tcp_new();  // Cria um novo bloco de controle TCP.
    tcp_bind(pcb, IP_ADDR_ANY, 80);   // Vincula à porta 80 (HTTP).
    pcb = tcp_listen(pcb);           // Escuta conexões de entrada.
    tcp_accept(pcb, http_server_accept);  // Define a função de aceitação.
}
void wifi_init(){
    printf("Iniciando WiFi\n");
    sleep_ms(50); 

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)) {  // Inicializa o módulo Wi-Fi.
        printf("Erro no WiFi\n");
        return;
    }

    cyw43_arch_enable_sta_mode();


    if (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK)) {
        printf("Falha no WiFi\n");
        oled_print(2,"precione o");
        oled_print(3,"botao reset");
        gpio_put(RED_LED, 1);
        while (1) {
            sleep_ms(10000);
        }
        return;
    }

    printf("WiFi Conectado!\n");
    gpio_put(GREEN_LED, 1);
    
    // Após conectar o Wi‑Fi, sincronize a hora via NTP e atualize o RTC
    //sync_ntp_and_set_rtc();
}
char* wifi_ip() {
    static char ip_str[16];  // String estática para retornar o IP
    const ip4_addr_t *ip = netif_ip4_addr(netif_list);
    if (ip != NULL) {
        snprintf(ip_str, sizeof(ip_str), "%s", ip4addr_ntoa(ip));
        printf("Endereço IP: %s\n", ip_str);
        return ip_str;
    } else {
        printf("Não foi possível obter o endereço IP.\n");
        return "IP indisponível";
    }
}
void wait_for_app_connection() {

    char ip_address[16];
    strcpy(ip_address, wifi_ip());
    
    oled_clear();
    oled_print(3, "Conecte ao app");
    oled_print(4, ip_address);

    printf("IP do dispositivo: %s\n", ip_address);
    printf("Aguardando conexão...\n");

    while (!client.connected) { // Simula espera até que a conexão seja estabelecida
        sleep_ms(1000); // Checagem a cada 1 segundo.
    }

    printf("App conectado!\n");
    gpio_put(GREEN_LED, 0); //Desligando led indicador 
}

void wifi_test() {
    oled_print(2, "Testando wifi");

    if (gpio_get(GREEN_LED)) {
        sleep_ms(2000);
        oled_print(4, "wifi ok");
        return;
    }

    sleep_ms(2000);
    oled_print(4, "erro no wifi");
}

// Envia uma resposta HTTP ao cliente.
static err_t http_server_send(void *arg, struct tcp_pcb *tpcb, struct pbuf *p) {
    tcp_write(tpcb, p->payload, p->len, TCP_WRITE_FLAG_COPY);
    return ERR_OK;
}

static err_t http_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    tcp_close(tpcb);
    return ERR_OK;
}

// Recebe dados do cliente HTTP
static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb); // Fecha conexão se não há mais dados
        return ERR_OK;
    }

    printf("Requisição recebida: %s\n", (char *)p->payload);

    // Processa a requisição
    char *req = (char *)p->payload;
    char response[256];

    // Endpoint: /connect?time=<HH:MM>
    if (strstr(req, "GET /connect")) {
        int hour, minute;
        if (sscanf(req, "GET /connect?time=%2hhu:%2hhu", &hour, &minute) == 2) {
            datetime_t new_time = {
                .year = 2025,  // Ano atual
                .month = 1,    // Mês atual
                .day = 24,     // Dia atual
                .dotw = 5,     // Dia da semana (opcional)
                .hour = hour,
                .min = minute,
                .sec = 0
            };

            rtc_set_datetime(&new_time);  // Define o horário no RTC
            sleep_ms(200);
            snprintf(response, sizeof(response), 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Hora configurada para %02d:%02d\n", hour, minute);
        } else {
            snprintf(response, sizeof(response), 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Já existe um usuário conectado.\n");
        }
    }

    // Endpoint: /addfeedingtime?time=<HH:MM>
    else if (strstr(req, "GET /addfeedingtime?time=")) {
        uint8_t hour, minute;
        if (sscanf(req, "GET /addfeedingtime?time=%2hhu:%2hhu", &hour, &minute) == 2) {
            if (hour < 24 && minute < 60) {

                add_feed(&fl,hour,minute);

                snprintf(response, sizeof(response), 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Horário %02d:%02d adicionado à lista de alimentações.\n", hour, minute);
            } else {
                snprintf(response, sizeof(response), 
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Erro: O horário deve estar no formato HH:MM e ser válido.\n");
            }
        } else {
            snprintf(response, sizeof(response), 
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Erro: Parâmetro 'time' inválido ou ausente.\n");
        }
    }

    // Endpoint: /removefeedingtime?time=<HH:MM>
    else if (strstr(req, "GET /removefeedingtime?time=")) {
        uint8_t hour, minute;
        if (sscanf(req, "GET /removefeedingtime?time=%2hhu:%2hhu", &hour, &minute) == 2) {
            if(client.next_feed.hour == hour && client.next_feed.minute == minute){
                client.next_feed.hour = 99;
                client.next_feed.minute = 0;
            }

            if (remove_feed(&fl, hour, minute)) {
                snprintf(response, sizeof(response), 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Horário %02d:%02d removido da lista de alimentações.\n", hour, minute);
            } else {
                snprintf(response, sizeof(response), 
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Erro: Horário %02d:%02d não encontrado na lista.\n", hour, minute);
            }
        } else {
            snprintf(response, sizeof(response), 
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n\r\n"
                "Erro: Parâmetro 'time' inválido ou ausente.\n");
        }
    }

    // Endpoint: /temp
    else if (strstr(req, "GET /temp")) {
        float current_temperature = get_temperatura();
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "%.2f", current_temperature);
    }

    // Endpoint: /rgb?freq=<valor>
    else if (strstr(req, "GET /rgb")) {
        uint8_t r, g, b;
        sscanf(req, "GET /rgb?r=%hhu&g=%hhu&b=%hhu",  &r, &g, &b);
        rgb_set_color(r, g, b); // Ajusta a cor do LED
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Cor RGB ajustada para: R=%d, G=%d, B=%d", r, g, b);
    }
    // Endpoint: /eat
    else if (strstr(req, "GET /eat")) {
        handle_feeding();
        save_data();
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Comi!!");
    }
    // Endpoint: /reset
    else if (strstr(req, "GET /reset")) {
        reset_client();
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Resetado!");
    }

    // Endpoint: /last
    else if (strstr(req, "GET /last")) {
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "%d:%d", client.last_feed.hour ,client.last_feed.minute);
    }
    // Endpoint: /next
    else if (strstr(req, "GET /next")) {
        find_next_feed(fl);
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "%d:%d", client.next_feed.hour ,client.next_feed.minute);
    }

    // Resposta padrão para endpoints não reconhecidos
    else {
        snprintf(response, sizeof(response), 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "Endpoint não encontrado.\n");
    }

    // Envia resposta ao cliente
    tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);  // Garante que a resposta seja enviada imediatamente
    pbuf_free(p);      // Libera o buffer recebido

    oled_clear();
    oled_print(3, "Requisicao");
    oled_print(4, "Recebida!");
    sleep_ms(1000);
    
    //Atualiza o display
    oled_clear();
    update_display_oled();

    if (strstr(req, "GET /connect")){
        find_next_feed(fl);
        client.connected = 1;
        save_data();
    }
    
    tcp_sent(tpcb, http_server_sent);

    return ERR_OK;
}


// Aceita novas conexões de clientes HTTP.
static err_t http_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
     
    if (connection_count >= MAX_CONNECTIONS) {
        printf("Limite de conexões atingido. Recusando nova conexão.\n");
        tcp_close(newpcb);
        return ERR_ABRT;
    }
    
    connection_count++;
    printf("Nova conexão aceita. Total de conexões: %d\n", connection_count);

    tcp_recv(newpcb, http_server_recv);
    tcp_err(newpcb, http_server_close);
    tcp_poll(newpcb, NULL, 10);  // Timeout de 10 segundos

    tcp_arg(newpcb, NULL); // Garante que não há ponteiros inválidos
    return ERR_OK;
}

static void http_server_close(void *arg, err_t err) {
    struct tcp_pcb *pcb = (struct tcp_pcb *)arg;
    if (pcb != NULL) {
        printf("Fechando conexão.\n");
        tcp_close(pcb);  // Fecha a conexão
    }
}



