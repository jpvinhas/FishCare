#ifndef OLED_H
#define OLED_H

#include "util.h"

#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Configuração do I2C e display OLED
#define I2C_PORT i2c0
#define I2C_SDA 14
#define I2C_SCL 15

#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SSD1306_BUFFER_SIZE (ssd1306_width * ssd1306_height / 8)

void oled_init();
void oled_clear();
void oled_print(int line, const char *message);
void oled_print_multiline(const char *message);

#endif
