#ifndef MEMORY_H
#define MEMORY_H

#include <string.h>  // Para memcpy e memset
#include <stdio.h>   // Para debug (printf)
#include <stdint.h> // Para uintptr_t

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "fishcare.h"

#define FLASH_TARGET_OFFSET (1536 * 1024) // Exemplo: 1.5 MB

void save_data();
void load_data();
void add_feeding_time(uint8_t hour, uint8_t minute);
void remove_feeding_time(uint8_t hour, uint8_t minute);
void init_client();
void reset_client();

#endif