#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "inc/bmp280.h"

#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3
#define I2C_ADDR 0x76

int main() {
 
    stdio_init_all();
    sleep_ms(5000);

    bmp280_init();

    while (true) {
        sensors_t s = bmp280_get_all(I2C_ADDR);
        
        float altitudeHypsometric = bmp280_calculate_altitude_hypsometric(I2C_ADDR, s.pressure, s.temperature);
        printf("Temp: %.2f °C | Pressure: %lu Pa | Altitude: %.2f m | Altitude Precisa: %.2f m \n",
               s.temperature, s.pressure, s.altitude, altitudeHypsometric);
        sleep_ms(1000);
    }

    return 0;
}