#include "temperatura.h"

//Temperatura
uint64_t addr_list[MAX_DEVICES];
uint device_count;
pico_1wire_t *ctx;

// Função para inicializar o sensor de temperatura
pico_1wire_t* init_temperature_sensor(uint64_t *addr_list, uint *device_count) {
    pico_1wire_t *ctx = pico_1wire_init(DATA_PIN, POWER_PIN, true);
    if (!ctx) {
        printf("Erro: pico_1wire_init() falhou.\n");
        return NULL;
    }

    printf("Verificando dispositivos no barramento...\n");

    int res = pico_1wire_search_rom(ctx, addr_list, MAX_DEVICES, device_count);
    if (res || *device_count == 0) {
        printf("Erro: nenhum dispositivo encontrado no barramento.\n");
        return NULL;
    }

    printf("Dispositivos encontrados: %u\n", *device_count);
    for (int i = 0; i < *device_count; i++) {
        printf("Dispositivo %02d: %016llX\n", i + 1, addr_list[i]);
    }

    return ctx;
}

// Função para ler a temperatura de um dispositivo específico
float read_temperature(pico_1wire_t *ctx, uint64_t addr) {
    uint conv_time;
    int res;

    // Obtém o tempo necessário para a conversão da temperatura
    res = pico_1wire_convert_duration(ctx, 0, &conv_time);
    if (res) {
        printf("Erro ao calcular duração da conversão: %d\n", res);
        return -999.0; // Valor de erro
    }

    // Inicia a conversão de temperatura
    res = pico_1wire_convert_temperature(ctx, 0, false);
    if (res) {
        printf("Erro ao converter temperatura: %d\n", res);
        return -999.0; // Valor de erro
    }

    // Aguarda a conversão ser concluída
    sleep_ms(conv_time);

    // Lê a temperatura do dispositivo
    float temp;
    res = pico_1wire_get_temperature(ctx, addr, &temp);
    if (res) {
        printf("Erro ao obter temperatura do dispositivo %016llX: %d\n", addr, res);
        return -999.0; // Valor de erro
    }

    return temp;
}
void temperatura_test() {
    oled_print(1, "Testando");
    oled_print(2, "Temperatura");

    float temp = get_temperatura();
    if (temp != -999.0) {
        printf("Temperatura atual: %.2f°C\n", temp);
    } else {
        printf("Erro ao buscar temperatura.\n");
    }

    sleep_ms(2000);
    oled_print(4, "ok");

    char temp_msg[20];
    sprintf(temp_msg, "Temp: %.2fC", temp);
    oled_print(5, temp_msg);
}

// Função para testar o sensor de temperatura
void test_temperature(pico_1wire_t *ctx, uint64_t *addr_list, uint device_count) {
    for (int i = 0; i < device_count; i++) {
        float temp = read_temperature(ctx, addr_list[i]);
        if (temp != -999.0) {
            printf("Dispositivo %016llX: %.2f°C\n", addr_list[i], temp);
        } else {
            printf("Erro ao ler dispositivo %016llX\n", addr_list[i]);
        }
    }
}

void temperatura_init(){
    printf("Iniciando Sensor de Temperatura...\n");
    // Reinicializa o barramento One-Wire caso o sensor trave
    if (ctx) {
        pico_1wire_destroy(ctx);  // Fecha o contexto anterior
    }
    ctx = init_temperature_sensor(addr_list, &device_count);
    if (!ctx) {
        printf("Falha ao inicializar o sensor de temperatura.\n");
    }
}

float get_temperatura(){

    printf("Buscando temperatura...\n");
    for (int i = 0; i < device_count; i++) {
        float temp = read_temperature(ctx, addr_list[i]);
        if (temp != -999.0) {
            printf("Dispositivo %016llX: %.2f°C\n", addr_list[i], temp);
            return temp;
        }
    }
}