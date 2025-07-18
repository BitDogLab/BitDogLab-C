/*
SENSOR DE DISTÂNCIA A LASER

O código abaixo foi desenvolvido com propósitos didáticos, para uso
ao longo do Curso de Capacitação em Sistemas Embarcados - Embarcatech.

Para usar o código abaixo, conecte o módulo VL53L0X (DISTANCIA),
usando um conector JST SH de 4 fios, ao port I2C 0 da BitDogLab.

A distância (em mm e m), a partir do sensor, pode ser lida via
Serial Monitor.
*/

// AVISO:
// O código abaixo foi feito para operação com o módulo VL53L0X.
// Portanto, pode não ser compatível com um módulo VL53L1X.

// Bibliotecas inclusas
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições de I2C
#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define VL53L0X_ADDR 0x29

// Lista de endereços de registradores
#define REG_IDENTIFICATION_MODEL_ID   0xC0
#define REG_SYSRANGE_START            0x00
#define REG_RESULT_RANGE_STATUS       0x14
#define REG_RESULT_RANGE_MM           0x1E

/*
--- CONFIGURAR I2C ---
    Função de inicialização e configuração geral do I2C.
*/
int config_i2c() {
    i2c_init(I2C_PORT, 100 * 1000); // Comunicação I2C a 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

/*
--- INICIALIZAÇÃO DO SENSOR ---
*/
int vl53l0x_init(i2c_inst_t *i2c) {
    uint8_t reg = REG_IDENTIFICATION_MODEL_ID;
    uint8_t id;

    // Lê ID do sensor
    if (i2c_write_blocking(i2c, VL53L0X_ADDR, &reg, 1, true) != 1)
        return 0;
    if (i2c_read_blocking(i2c, VL53L0X_ADDR, &id, 1, false) != 1)
        return 0;
    if (id != 0xEE) {       // Confirma se é o VL53L0X
        printf("ID inválido: 0x%02X (esperado: 0xEE)\n", id);
        return 0;
    }

    return 1;
}

/*
--- LEITURA DO SENSOR VL53L0X ---
*/
int vl53l0x_read_distance_mm(i2c_inst_t *i2c) {
    // Inicia medição única
    uint8_t cmd[2] = {REG_SYSRANGE_START, 0x01};
    if (i2c_write_blocking(i2c, VL53L0X_ADDR, cmd, 2, false) != 2)
        return -1;

    // Aguarda resultado (~50 ms máx)
    for (int i = 0; i < 100; i++) {
        uint8_t reg = REG_RESULT_RANGE_STATUS;
        uint8_t status;

        if (i2c_write_blocking(i2c, VL53L0X_ADDR, &reg, 1, true) != 1)
            return -1;
        if (i2c_read_blocking(i2c, VL53L0X_ADDR, &status, 1, false) != 1)
            return -1;

        if (status & 0x01) break;  // Medição pronta
        sleep_ms(5);
    }

    // Lê os 2 bytes da distância
    uint8_t reg = REG_RESULT_RANGE_MM;
    uint8_t buffer[2];

    if (i2c_write_blocking(i2c, VL53L0X_ADDR, &reg, 1, true) != 1)
        return -1;
    if (i2c_read_blocking(i2c, VL53L0X_ADDR, buffer, 2, false) != 2)
        return -1;

    return (buffer[0] << 8) | buffer[1];
}

/*
--- FUNÇÃO PRINCIPAL ---
*/
int main() {
    stdio_init_all();   // Inicialização geral
    sleep_ms(3000);
    printf("VL53L0X - Leitura de Distância (Laser)\n");

    config_i2c();       // Configuração de I2C
    sleep_ms(3000);

    if (!vl53l0x_init(I2C_PORT)) {
        printf("Falha ao inicializar o VL53L0X.\n");
        return 1;
    }

    printf("Sensor iniciado com sucesso.\n");

    while (1) {         // Imprimir distância lida
        int distancia = vl53l0x_read_distance_mm(I2C_PORT);
        if (distancia < 0) {
            printf("Erro na leitura da distância.\n");
        } else {
            printf("Distância: %d mm (%.2f m)\n", distancia, distancia / 1000.0f);
        }
        sleep_ms(500);
    }

    return 0;
}
