#ifndef FISHCARE_H
#define FISHCARE_H

#include "util.h"
#include "wifi.h"
#include "motor.h"
#include "temperatura.h"
#include "memory.h"

// Máximo de horários armazenados
#define MAX_FEEDS 10  

// Estruturas usados no projeto
typedef struct {
    uint8_t hour;
    uint8_t minute;
}FeedingTime;

typedef struct FishClient{
    int remainingFeedings;
    int scheduleCount;
    int connected;
    FeedingTime feedingSchedule[MAX_FEEDS];
    FeedingTime last_feed;
    FeedingTime next_feed;
}FishClient;

typedef struct FeedingList{
    FeedingTime time;
    struct FeedingList *prox;
}FeedingList;

//Estrutura do cliente, guarda as informações na memória flash
extern FishClient client;

//Lista encadeada com os horários ordenados
extern FeedingList *fl;

void fishcare_mode(); //Função principal do modo FishCare

void handle_feeding();
void add_feed(FeedingList **lista, uint8_t hour, uint8_t minute);
bool remove_feed(FeedingList **lista, uint8_t hour, uint8_t minute);
void print_feed_list(FeedingList *lista);
void update_display_oled();
void feeding_check_task();
void update_display_task();
void update_feed_list();
void find_next_feed(FeedingList *lista);

#endif