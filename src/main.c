// Bibliotecas padrão
#include <stdio.h>
#include "pico/stdlib.h"

//Bibliotecas do projeto
#include "fishcare.h"
#include "motor.h"
#include "oled.h"
#include "rgb.h"
#include "util.h"
#include "wifi.h"
#include "temperatura.h"
#include "memory.h"

//Funces
void setup();
void test_demo();
void test_mode();

int main() {
    
    setup();

    oled_clear();
    oled_print(2, "A - Testes");
    oled_print(4, "B - FishCare");

    // Imprime a fila ordenada
    print_feed_list(fl);

    // Seleção do modo
    while (1) {

        if (!gpio_get(BUTTON_A)) {
            test_mode();
        } else if (!gpio_get(BUTTON_B)) {
            fishcare_mode();
        }

        sleep_ms(100); // Debounce dos botões
    }
    
    return 0;
}


void setup() {

    // Inicializa a comunicação serial (para debug).
    stdio_init_all();
    sleep_ms(1000);

    // Inicializa o display
    oled_init();

    printf("Iniciando Sistema...\n");
    oled_print(3," BEM VINDO!");

    // Inicializa os periféricos
    motor_init();
    rgb_init();
    rtc_init_config();
    rgb_led_init();
    buttons_init();
    wifi_init();
    server_init();
    temperatura_init();
    init_client();

    printf("Sistema iniciado...\n");
    sleep_ms(1000);
}

void test_demo() {
    oled_clear();
    oled_print(2,"Modo Demo");

    // Inicia as threads para o motor e o RGB
    multicore_launch_core1(motor_thread); // Motor na Core 1
    rgb_thread();                         // RGB na Core 0
}

void test_mode() {

    oled_clear();
    
    // Testes individuais
    motor_test();
    sleep_ms(1000);
    oled_clear();

    rgb_test();
    sleep_ms(1000);
    oled_clear();

    wifi_test();
    sleep_ms(1000);
    oled_clear();

    temperatura_test();
    sleep_ms(2000);
    oled_clear();

    // Entrar no modo contínuo
    test_demo();
}

