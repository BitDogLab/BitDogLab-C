#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define I2C_SDA 2
#define I2C_SCL 3

#define I2C_ADDR 0x38

void config_i2c() {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}


void init_sensor() {
    uint8_t init_cmd = 0xE1; 
    i2c_write_blocking(I2C_PORT, I2C_ADDR, &init_cmd, 1, false);
}

void read_measurement(uint8_t *data) {

    uint8_t trigger_cmd = 0xAC;
    static uint8_t i = 5;
    i+=5;
    // Read 6 bytes: 3 for humidity, 3 for temperature
    i2c_write_blocking(I2C_PORT, I2C_ADDR, &trigger_cmd, 1, false);
    sleep_ms(i);
    i2c_read_blocking(I2C_PORT, I2C_ADDR, data, 6, false);
    printf("Espera = %d\n", i);
}

int main()
{
    uint8_t data[6];
    uint8_t raw;

    stdio_init_all();
    config_i2c();
    sleep_ms(5000);
    init_sensor();
    sleep_ms(1000);
    i2c_read_blocking(I2C_PORT, I2C_ADDR, &raw, 1, false);
    printf("Teste 0: %x\n", raw);
    
    while(true) {
        sleep_ms(1000);
        read_measurement(data);
        printf("Teste dados: %x %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4], data[5]);

    }

}
