#ifndef WIFI_H
#define WIFI_H

// Habilita Conexão Wifi
#include "pico/cyw43_arch.h"

// Comunicação TCP/IP
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"

#include "util.h"
#include "oled.h"
#include "fishcare.h"
#include "rgb.h"
#include "temperatura.h"

// Conexão Wifi
#define WIFI_SSID "iPhone do Jp"
#define WIFI_PASSWORD "88888888"

// Pinos LED RGB interno da Placa
#define GREEN_LED 11 
#define BLUE_LED 12
#define RED_LED 13

typedef struct CurrentTime{
    int hour;
    int minute;
    int sec;
}CurrentTime;

//Controle de acesso simultâneo
extern int connection_count;
static const int MAX_CONNECTIONS = 25;  // Limite de conexões simultâneas.

void server_init();
void wifi_init();
char* wifi_ip();
void wait_for_app_connection();
void wifi_test();
static err_t http_server_send(void *arg, struct tcp_pcb *tpcb, struct pbuf *p);
static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t http_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static void http_server_close(void *arg, err_t err);


#endif
