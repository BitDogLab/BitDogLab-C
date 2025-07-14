/*
SENSOR DE LUMINOSIDADE

O código abaixo foi desenvolvido com propósitos didáticos, para uso
ao longo do Curso de Capacitação em Sistemas Embarcados - Embarcatech.

Para usar o código abaixo, conecte o módulo SENSOR DE LUMINOSIDADE
(LUMINOS), usando um conector JST SH de 4 fios, ao port I2C 0 da
BitDogLab.

Os valores medidos (em lux) podem ser lidos via Serial Monitor.
*/

// Bibliotecas inclusas
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições de I2C
#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define BH1750_ADDR 0x23 // ou 0x5C, dependendo do sensor
#define BH1750_CONT_HRES_MODE 0x10 // Modo contínuo, alta resolução

/*
--- CONFIGURAR I2C ---
    Função de inicialização e configuração geral do I2C.
*/
void config_i2c() {
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

/*
--- INICIALIZAR SENSOR BH ---
    Inicializa o BH1750 em modo contínuo de medição de alta resolução.
*/
void bh1750_init() {
    uint8_t cmd = BH1750_CONT_HRES_MODE;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, &cmd, 1, false);
}

/*
--- LEITURA DO SENSOR BH1750 ---
    Lê 2 bytes da medição de iluminância (16 bits) diretamente do
    sensor, e converte o valor bruto para lux, usando fator de escala
    padrão (1.2).
*/
uint16_t bh1750_read_lux() {
    uint8_t data[2];
    i2c_read_blocking(I2C_PORT, BH1750_ADDR, data, 2, false);
    uint16_t raw = (data[0] << 8) | data[1];
    // Conversão: 1 lux = raw / 1.2
    return raw / 1.2;
}

/*
--- FUNÇÃO PRINCIPAL ---
*/
int main() {
    stdio_init_all();   // Inicialização geral
    config_i2c();       // Configuração de I2C

    sleep_ms(3000);     // Aguarda USB serial iniciar
    printf("Iniciando sensor BH1750...\n");

    bh1750_init();      // Inicialização do sensor
    sleep_ms(180);      // Tempo de integração do sensor

    while (1) {

        uint16_t lux = bh1750_read_lux();
        printf("Luminosidade: %d lux\n", lux);
        sleep_ms(1000);     // Leitura a cada 1 segundo
    }

    return 0;
}
