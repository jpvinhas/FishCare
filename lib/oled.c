#include "oled.h"


void oled_init() {

    printf("Iniciando OLED...\n");
    
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();
    printf("OLED OK!\n");
    
    // Limpa o buffer, calcula area do frame e envia para o display
    oled_clear();
}

// Limpa o display e desativa
void oled_clear() {
    // Define a área de renderização do display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    // Calcula o comprimento do buffer necessário para renderizar
    calculate_render_area_buffer_length(&frame_area);

    // Limpa o buffer do display
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);

    render_on_display(buffer, &frame_area);  // Apaga o display
}

// Função para exibir mensagens em uma linha especifica, sem alterar as outras linhas
void oled_print(int line, const char *message) {
    // Define a área de renderização do display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = line,
        .end_page = line
    };

    // Calcula o comprimento do buffer necessário para renderizar
    calculate_render_area_buffer_length(&frame_area);

    // Limpa o buffer do display
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);

    // Calcula a largura do texto
    int char_width = 8;  // Largura do caractere em pixels
    int message_length = strlen(message);
    int text_width = message_length * char_width;

    // Calcula a posição horizontal (x) para centralizar o texto
    int x = (ssd1306_width - text_width) / 2;
    if (x < 0) x = 0;

    // Desenha a mensagem no buffer (conversão explícita para remover const)
    ssd1306_draw_string(buffer, x, line, (char *)message);

    // Renderiza o buffer no display
    render_on_display(buffer, &frame_area);
}

// Exibe texto com quebra automática de linha
void oled_print_multiline(const char *message) {
    // Define a área de renderização do display
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    // Calcula o comprimento do buffer necessário para renderizar
    calculate_render_area_buffer_length(&frame_area);

    // Limpa o buffer do display
    uint8_t buffer[ssd1306_buffer_length];
    memset(buffer, 0, ssd1306_buffer_length);

    int line = 1;                       // Linha atual no display
    int char_count = 0;                 // Contador de caracteres na linha
    char line_buffer[17] = {0};         // Buffer para até 16 caracteres (+1 para o terminador nulo)

    for (int i = 0; message[i] != '\0'; i++) {
        if (char_count >= 16 || message[i] == '\n') {  // Quebra de linha ao atingir o limite ou '\n'
            line_buffer[char_count] = '\0';            // Finaliza a linha atual
            ssd1306_draw_string(buffer, 0, line * 8, line_buffer);  // Desenha a linha no display
            line++;
            char_count = 0;                            // Reseta o contador de caracteres

            // Verifica se o display está cheio
            if (line >= (ssd1306_n_pages)) break;
        }

        if (message[i] != '\n') {
            if (char_count < 16) {                     // Evita estouro de buffer
                line_buffer[char_count++] = message[i];
            }
        }
    }

    // Desenha a última linha, se necessário
    if (char_count > 0 && line < ssd1306_n_pages) {
        line_buffer[char_count] = '\0';
        ssd1306_draw_string(buffer, 0, line * 8, line_buffer);
    }

    // Renderiza o buffer no display
    render_on_display(buffer, &frame_area);
}