#include "bmp280.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "math.h"

#define I2C_PORT i2c1
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

void bmp280_write_array(uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength)
{
    uint8_t buffer[dataLength + 1];
    buffer[0] = startRegisterAddress;
    for (uint8_t i = 0; i < dataLength; i++) {
        buffer[i + 1] = data[i];
    }

    i2c_write_blocking(I2C_PORT, deviceAddress, buffer, dataLength + 1, false);
}

void bmp280_read_array(uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength)
{
    // Escreve o endereço do registrador a ser lido
    i2c_write_blocking(I2C_PORT, deviceAddress, &startRegisterAddress, 1, true);
    // Lê os dados a partir dele
    i2c_read_blocking(I2C_PORT, deviceAddress, data, dataLength, false);
}

void bmp280_i2c_init()
{
    // Inicializa o I2C0 com frequência de 100kHz
    i2c_init(I2C_PORT, 100 * 1000);
    
    // Configura os pinos GPIO para função I2C
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Ativa os resistores de pull-up internos
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

void delay_function(uint32_t delayMS)
{
    sleep_ms(delayMS);
}

float power_function(float x, float y)
{
    return powf(x, y); // Usando função padrão da math.h
}
