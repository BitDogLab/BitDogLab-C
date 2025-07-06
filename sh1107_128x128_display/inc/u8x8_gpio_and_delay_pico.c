

#include "pico/stdlib.h"
#include <u8g2.h>

// Defina os pinos usados para SDA e SCL
#define I2C_SDA 2
#define I2C_SCL 3

uint8_t u8x8_gpio_and_delay_pico(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            gpio_init(I2C_SCL);
            gpio_init(I2C_SDA);
            gpio_set_dir(I2C_SCL, GPIO_OUT);
            gpio_set_dir(I2C_SDA, GPIO_OUT);
            break;

        case U8X8_MSG_DELAY_MILLI:
            sleep_ms(arg_int);
            break;

        case U8X8_MSG_DELAY_10MICRO:
            sleep_us(10 * arg_int);
            break;

        case U8X8_MSG_DELAY_100NANO:
            sleep_us(1);  // Pico não tem precisão menor que 1µs
            break;

        case U8X8_MSG_GPIO_I2C_CLOCK:
            gpio_put(I2C_SCL, arg_int);
            break;

        case U8X8_MSG_GPIO_I2C_DATA:
            gpio_put(I2C_SDA, arg_int);
            break;

        default:
            return 0;
    }
    return 1;
}