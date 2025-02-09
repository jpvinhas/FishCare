#include "util.h"


void rgb_led_init(){
    printf("Iniciando LED...\n");
    // Inicializa os pinos RGB
    gpio_init(RED_LED);
    gpio_init(BLUE_LED);
    gpio_init(GREEN_LED);

    // Define os pinos como saída
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_set_dir(BLUE_LED, GPIO_OUT);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
}

void buttons_init(){
    printf("Iniciando Botoes...\n");
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}
void rtc_init_config() {
    // Ativa o oscilador externo
    rtc_init();
    sleep_ms(50); // Aguarda estabilização do oscilador
    rtc_set_datetime(&(datetime_t){
        .year = 2025,
        .month = 1,
        .day = 24,
        .dotw = 5,  // Dia da semana (0 = domingo, 5 = sexta-feira)
        .hour = 12,
        .min = 0,
        .sec = 0
    });
    sleep_ms(50);
}
void get_current_time(uint8_t *hour, uint8_t *minute) {
    printf("aquu!!");   

    datetime_t t;
    if (rtc_get_datetime(&t)) {  // Verifica se o RTC retorna valores válidos
        *hour = t.hour;
        *minute = t.min;
    } else {
        *hour = 0;
        *minute = 0;  // Fallback em caso de erro
    }
}
// Função para comparar dois horários
int compare_time(uint8_t hour1, uint8_t minute1, uint8_t hour2, uint8_t minute2) {
    if (hour1 < hour2 || (hour1 == hour2 && minute1 < minute2)) {
        return -1; // Primeiro horário é menor
    } else if (hour1 == hour2 && minute1 == minute2) {
        return 0; // Horários iguais
    }
    return 1; // Primeiro horário é maior
}

// Função que sincroniza via NTP e atualiza o RTC com a hora obtida
void sync_ntp_and_set_rtc() {
    // Configura o SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    
    ip_addr_t ntp_server;
    IP4_ADDR(&ntp_server, 91, 189, 94, 4); // Exemplo de um servidor pool.ntp.org
    sntp_setserver(0, &ntp_server);
    
    sntp_init();

    // Aguarda a sincronização NTP, forçando o polling do lwIP
    struct timeval tv;
    int retries = 0;
    while (retries < NTP_SYNC_RETRY) {
        // Força o processamento das tarefas de rede
        cyw43_arch_poll();

        gettimeofday(&tv, NULL);
        int ret = gettimeofday(&tv, NULL);
        printf("gettimeofday retornou: %d, tv.tv_sec: %ld\n", ret, tv.tv_sec);  
        printf("tv.tv_sec atual: %ld\n", tv.tv_sec);
        if (tv.tv_sec > TIME_THRESHOLD) {
            printf("Sincronização NTP bem sucedida: %ld\n", tv.tv_sec);
            break;
        }
        printf("Aguardando sincronização NTP...\n");
        sleep_ms(1000);
        retries++;
    }

    if (retries >= NTP_SYNC_RETRY) {
        printf("Falha na sincronização NTP.\n");
        return;
    }

    // Converte o Unix time (tv.tv_sec) para a estrutura datetime_t
    struct tm *tm_info = gmtime(&tv.tv_sec);
    if (tm_info == NULL) {
        printf("Erro ao converter o tempo via gmtime.\n");
        return;
    }

    datetime_t dt = {
        .year = tm_info->tm_year + 1900,
        .month = tm_info->tm_mon + 1,
        .day = tm_info->tm_mday,
        .dotw = tm_info->tm_wday,
        .hour = tm_info->tm_hour,
        .min = tm_info->tm_min,
        .sec = tm_info->tm_sec
    };

    // Atualiza o RTC com o novo horário
    rtc_set_datetime(&dt);
    printf("RTC atualizado via NTP: %d-%02d-%02d %02d:%02d:%02d\n",
           dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
}