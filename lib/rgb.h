#ifndef RGB_H
#define RGB_H

#include "oled.h"
#include "util.h"

// Defina os GPIOs usados para FITA R, G e B
#define RED_PIN 8
#define GREEN_PIN 20
#define BLUE_PIN 9

void rgb_init();
void setup_pwm(uint gpio);
void rgb_set_color(uint8_t r, uint8_t g, uint8_t b);
void rgb_test(); 
void rgb_thread();

#endif
