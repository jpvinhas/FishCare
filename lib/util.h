#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <time.h>
#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"
#include "lwip/apps/sntp.h"

#include "hardware/rtc.h"
#include "hardware/pwm.h"

#include "inc/pico_1wire.h"

// Pinos LED RGB interno da Placa
#define GREEN_LED 11 
#define BLUE_LED 12
#define RED_LED 13

// GPIO dos botões
#define BUTTON_A 5
#define BUTTON_B 6

// Define quantas tentativas faremos para aguardar a sincronização NTP
#define NTP_SYNC_RETRY 30
// Data limite para considerar que a sincronização ocorreu (01/01/2020 em Unix time)
#define TIME_THRESHOLD 1577836800

// Estado do LED
static bool led_state = false;

void rgb_led_init();
void buttons_init();
void rtc_init_config();
void get_current_time(uint8_t *hour, uint8_t *minute);
int compare_time(uint8_t hour1, uint8_t minute1, uint8_t hour2, uint8_t minute2);
void sync_ntp_and_set_rtc();

#endif