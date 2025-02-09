#ifndef MOTOR_H
#define MOTOR_H

#include "util.h"
#include "oled.h"

// Pinos do motor de passo
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19

// Número de passos na sequência
#define STEP_COUNT 4

void motor_init();
void motor_step(int step);
void motor_move(int steps, int delay_ms);
void motor_test();
void motor_thread();

#endif
