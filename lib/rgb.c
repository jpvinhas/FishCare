#include "rgb.h"

void rgb_init(){
    // Inicializa PWM nos pinos da fita RGB
    setup_pwm(RED_PIN);
    setup_pwm(GREEN_PIN);
    setup_pwm(BLUE_PIN);

    // Garante que os canais estejam desligados inicialmente
    pwm_set_gpio_level(RED_PIN, 0);
    pwm_set_gpio_level(GREEN_PIN, 0); // Certifica-se de que GREEN_PIN começa apagado
    pwm_set_gpio_level(BLUE_PIN, 0);
}

void setup_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, 255); // 8 bits de resolução
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), 0);
    pwm_set_enabled(slice_num, true);
}

void rgb_set_color(uint8_t r, uint8_t g, uint8_t b) {
    pwm_set_gpio_level(RED_PIN, r);
    pwm_set_gpio_level(GREEN_PIN, g);
    pwm_set_gpio_level(BLUE_PIN, b);
}

void rgb_test() {

    oled_print(2, "Testando RGB");

    pwm_set_gpio_level(RED_PIN, 0);
    pwm_set_gpio_level(GREEN_PIN, 255);
    pwm_set_gpio_level(BLUE_PIN, 0);
    sleep_ms(500); // Verde

    pwm_set_gpio_level(RED_PIN, 255);
    pwm_set_gpio_level(GREEN_PIN, 0);
    pwm_set_gpio_level(BLUE_PIN, 0);
    sleep_ms(500); // Vermelho

    pwm_set_gpio_level(RED_PIN, 0);
    pwm_set_gpio_level(GREEN_PIN, 0);
    pwm_set_gpio_level(BLUE_PIN, 255);
    sleep_ms(500); // Azul

    for (int i = 0; i < 256; i++) {
        pwm_set_gpio_level(RED_PIN, i);
        pwm_set_gpio_level(GREEN_PIN, 255 - i);
        pwm_set_gpio_level(BLUE_PIN, (i / 2));
        sleep_ms(10);
    }
    for (int i = 255; i >= 0; i--) {
        pwm_set_gpio_level(RED_PIN, i);
        pwm_set_gpio_level(GREEN_PIN, 255 - i);
        pwm_set_gpio_level(BLUE_PIN, (i / 2));
        sleep_ms(10);
    }

    // Certifica-se de que todos os canais estejam apagados ao final do teste
    pwm_set_gpio_level(RED_PIN, 0);
    pwm_set_gpio_level(GREEN_PIN, 0);
    pwm_set_gpio_level(BLUE_PIN, 0);

    oled_print(4, "RGB OK");
}

void rgb_thread() {
    int rgb_counter = 0;
    while (true) {
        pwm_set_gpio_level(RED_PIN, rgb_counter);
        pwm_set_gpio_level(GREEN_PIN, 255 - rgb_counter);
        pwm_set_gpio_level(BLUE_PIN, (rgb_counter / 2));
        rgb_counter = (rgb_counter + 1) % 256;
        sleep_ms(15); // Tempo de transição do RGB
    }
}
