#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h" 

#define BUTTON_A 5
#define BUTTON_B 6

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C

volatile int counter = 0;
volatile int click_count = 0;
volatile bool counting = false;
volatile bool start_requested = false;
volatile bool update_requested = false;

// Buffer para o display OLED
uint8_t ssd[ssd1306_buffer_length];

// Protótipos
void gpio_callback(uint gpio, uint32_t events);
void start_countdown();
void update_display();
void ssd1306_clear();
void ssd1306_show();

// Área de renderização para o display
struct render_area frame_area;

int main() {
    stdio_init_all();
    sleep_ms(2000); // Espera 2 segundos para estabilizar a USB
    printf("Sistema iniciado...\n");

    // Inicializa botões
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    // Inicializa I2C
    i2c_init(I2C_PORT, ssd1306_i2c_clock * 1000); // 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa display OLED
    ssd1306_init(I2C_PORT, OLED_ADDR, 128, 64);
    
    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    frame_area = (struct render_area){
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };
    ssd1306_clear();
    ssd1306_show();
    
    while (1) {
        if (start_requested) {
            start_requested = false;
            counting = true;
            start_countdown();
        }
        if (update_requested) {
            update_requested = false;
            update_display();
        }
        tight_loop_contents();  // ou outras tarefas
    }
    
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A && !counting) {
        counter = 9;
        click_count = 0;
        start_requested = true;  // apenas sinaliza
    } else if (gpio == BUTTON_B && counting) {
        click_count++;
        update_requested = true;  // apenas sinaliza
    }
}

void start_countdown() {
    while (counting && counter > 0) {
        update_display();
        sleep_ms(1000);
        counter--;
    }
    counting = false;
    update_display();
}

void update_display() {
    char buffer[32];
    ssd1306_clear();
    
    sprintf(buffer, "Contador: %d", counter);
    printf("%s\n", buffer);
    ssd1306_draw_string(ssd, 0, 0, buffer);
    
    sprintf(buffer, "Clicks: %d", click_count);
    printf("%s\n", buffer);
    ssd1306_draw_string(ssd, 0, 16, buffer);
    
    ssd1306_show();
}

// Zera o display inteiro
void ssd1306_clear() {
    memset(ssd, 0, ssd1306_buffer_length); // limpa o buffer manualmente
    render_on_display(ssd, &frame_area);
}

void ssd1306_show() {
    render_on_display(ssd, &frame_area);
}