#include <stdio.h>
#include "pico/stdlib.h"
#include <u8g2.h>

// Declara a função de GPIO/delay que você implementou
extern uint8_t u8x8_gpio_and_delay_pico(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

// Cria a estrutura do display
u8g2_t u8g2;

int main() {

    // Inicializa a estrutura U8g2 para o display SSD1327 128x128 I2C
    u8g2_Setup_sh1107_i2c_seeed_128x128_f(
        &u8g2,
        U8G2_R0,                     // rotação normal
        u8x8_byte_sw_i2c,           // I2C por software
        u8x8_gpio_and_delay_pico    // GPIO/delay handler para Pico
    );

    u8g2_InitDisplay(&u8g2);        // Inicializa o display
    u8g2_SetPowerSave(&u8g2, 0);    // Liga o display

    // Loop principal
    while (1) {
        u8g2_ClearBuffer(&u8g2);                      // Limpa o buffer interno
        u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);    // Define uma fonte
        u8g2_DrawStr(&u8g2, 10, 20, "Hello, World!"); // Escreve na posição (10,20)
        u8g2_SendBuffer(&u8g2);                       // Envia o buffer para o display

        sleep_ms(1000); 
        u8g2_DrawStr(&u8g2, 10, 40, "Ola, Mundo!"); // Escreve na posição (10,20)
        u8g2_SendBuffer(&u8g2); 
        sleep_ms(1000);
        
        u8g2_ClearBuffer(&u8g2);
        u8g2_DrawBox(&u8g2, 10, 10, 50, 50); // Retângulo preto
        u8g2_SendBuffer(&u8g2);
        sleep_ms(1000);
    }

    return 0;
}