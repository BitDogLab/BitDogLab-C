// Exemplo prático (em construção) na BitDogLab:
// - Uma interrupção de botão (GPIO) sinaliza um evento com uma flag.
// - O loop principal (executor cíclico) verifica essa flag e acende o LED RGB.
// - Uma tarefa do RTOS cuida da atualização do display OLED e comunicação USB.

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ssd1306.h"   // Supomos uma lib para display OLED SSD1306
#include "usb_comm.h"  // Supomos lib para comunicação USB

#define BUTTON_PIN 15
#define LED_R_PIN 16
#define LED_G_PIN 17
#define LED_B_PIN 18

volatile bool button_pressed_flag = false;  // Flag global, modificada pela interrupção

// Interrupção do botão
void button_isr(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        button_pressed_flag = true;
    }
}

// Tarefa do RTOS para Display OLED + USB
void vTask_DisplayAndUSB(void *pvParameters) {
    ssd1306_init();  // Inicializa o display OLED
    usb_comm_init(); // Inicializa a comunicação USB

    while (1) {
        ssd1306_clear();
        ssd1306_draw_string(0, 0, "BitDogLab: Rodando");
        ssd1306_update();

        usb_comm_send("Status: OK\n");

        vTaskDelay(pdMS_TO_TICKS(1000));  // Atualiza a cada 1 segundo
    }
}

// Inicializa LEDs
void init_leds(void) {
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
}

// Inicializa botão com interrupção
void init_button(void) {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_isr);
}

int main() {
    stdio_init_all();
    init_leds();
    init_button();

    // Cria a tarefa de Display e USB
    xTaskCreate(vTask_DisplayAndUSB, "Display_USB", 1024, NULL, 1, NULL);

    // Start scheduler
    vTaskStartScheduler();

    while (true) {
        // Nunca deve chegar aqui com RTOS rodando
    }
}

// Hook para loop ocioso, opcionalmente usado se quiser um "executor cíclico"
void vApplicationIdleHook(void) {
    // Loop principal: verifica flag e acende LED
    if (button_pressed_flag) {
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);

        // Reseta a flag
        button_pressed_flag = false;
    } else {
        // LED apagado
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);
    }
}
