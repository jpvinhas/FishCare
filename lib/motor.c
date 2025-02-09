#include "motor.h"

// Sequência para controlar o motor (onda completa)
int step_sequence[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

// Função para configurar os pinos do motor
void motor_init() {
    printf("Iniciando motor...\n");
    gpio_init(IN1);
    gpio_init(IN2);
    gpio_init(IN3);
    gpio_init(IN4);
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_set_dir(IN4, GPIO_OUT);
}

// Função para executar um passo do motor
void motor_step(int step) {
    gpio_put(IN1, step_sequence[step][0]);
    gpio_put(IN2, step_sequence[step][1]);
    gpio_put(IN3, step_sequence[step][2]);
    gpio_put(IN4, step_sequence[step][3]);
}

// Função para mover o motor em um sentido
void motor_move(int steps, int delay_ms) {
    for (int i = 0; i < steps; i++) {
        for (int step = 0; step < STEP_COUNT; step++) {
            motor_step(step);
            sleep_ms(delay_ms); // Delay entre os passos
        }
    }
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);
}

void motor_test() {
    oled_print(2, "Testando motor");
    sleep_ms(1000);
    
    motor_move(512, 2);  // Move 512 passos com 2 ms de intervalo
    sleep_ms(500);       // Pausa breve
    motor_move(-512, 2); // Move 512 passos no sentido contrário
    sleep_ms(500);

    oled_print(4, "Motor OK");
    sleep_ms(500);
}
void motor_thread() {
    int motor_step_counter = 0;
    while (true) {
        motor_step(motor_step_counter % STEP_COUNT); // Movimenta o motor
        motor_step_counter++;
        sleep_ms(2); // Aumenta a velocidade (ajustável)
    }
}
