#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "inc/bmp280.h"

#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3
#define I2C_ADDR 0x76

i2c_inst_t* bmp280_i2c = i2c1;

void i2c_scan_bus() {
    printf("Scanner I2C:\n");
    for (int addr = 0; addr < 128; ++addr) {
        uint8_t dummy = 0;
        int ret = i2c_write_timeout_us(i2c1, addr, &dummy, 1, false, 5000);
        if (ret >= 0) {
            printf("Dispositivo encontrado no endereço 0x%02x\n", addr);
        }
    }
}

int main() {
 
    stdio_init_all();
    
    sleep_ms(5000);
    bmp280_init();

    while (true) {
        sensors_t s = bmp280_get_all(I2C_ADDR);
        i2c_scan_bus();
        
        float altitudeHypsometric = bmp280_calculate_altitude_hypsometric(I2C_ADDR, s.pressure, s.temperature);
        printf("Temp: %.2f °C | Pressure: %lu Pa | Altitude: %.2f m | Altitude Precisa: %.2f m \n",
               s.temperature, s.pressure, s.altitude, altitudeHypsometric);

            sleep_ms(1000);
    }

    return 0;
}