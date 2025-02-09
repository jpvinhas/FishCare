#ifndef TEMPERATURA_H
#define TEMPERATURA_H

#include "util.h"
#include "oled.h"

#define MAX_DEVICES 5
#define DATA_PIN 4
#define POWER_PIN -1

//Temperatura
extern uint64_t addr_list[MAX_DEVICES];
extern uint device_count;
extern pico_1wire_t *ctx;

// Inicialização do sensor de temperatura
pico_1wire_t* init_temperature_sensor(uint64_t *addr_list, uint *device_count);

// Leitura de temperatura (para um endereço específico)
float read_temperature(pico_1wire_t *ctx, uint64_t addr);

// Teste do sensor de temperatura (para múltiplos dispositivos)
void temperatura_test();
void test_temperature(pico_1wire_t *ctx, uint64_t *addr_list, uint device_count);

void temperatura_init();
float get_temperatura();

#endif // TEMPERATURA_H