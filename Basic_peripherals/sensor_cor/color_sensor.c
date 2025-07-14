/*
SENSOR DE COR

O código abaixo foi desenvolvido com propósitos didáticos, para uso
ao longo do Curso de Capacitação em Sistemas Embarcados - Embarcatech.

Para usar o código abaixo, conecte o módulo SENSOR DE COR (SENS COR),
usando um conector JST SH de 4 fios, ao port I2C 0 da BitDogLab.

Os valores medidos e a cor reconhecida podem ser lidos via Serial Monitor.
*/

// Bibliotecas inclusas
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições de I2C
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define TCS34725_ADDRESS 0x29
#define COMMAND_BIT 0x80

#define ENABLE_REG 0x00
#define ATIME_REG  0x01
#define CONTROL_REG 0x0F

#define CDATAL 0x14  // Limpar canal
#define RDATAL 0x16
#define GDATAL 0x18
#define BDATAL 0x1A

/*
--- FUNÇÃO DE MÁXIMO ---
    Para definir o máximo entre dois inteiros (16 bits). Função
    matemática usada no algoritmo de detecção de cor.
*/
uint16_t max (uint16_t a, uint16_t b) {
    return (a > b)?a:b;
}

/*
--- CONFIGURAR I2C ---
    Função de inicialização e configuração geral do I2C.
*/
void config_i2c() {
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

/*
--- ESCRITA DO SENSOR TCS ---
    Função auxiliar para escrita (8 bits) no registrador do sensor.
*/
void tcs_write8(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = { COMMAND_BIT | reg, value };
    i2c_write_blocking(i2c0, TCS34725_ADDRESS, buffer, 2, false);
}

/*
--- LEITURA DO SENSOR TCS ---
    Função auxiliar para leitura (16 bits) do registrador do sensor.
*/
uint16_t tcs_read16(uint8_t reg) {
    uint8_t buffer[2];
    uint8_t command = COMMAND_BIT | reg;
    i2c_write_blocking(i2c0, TCS34725_ADDRESS, &command, 1, true);
    i2c_read_blocking(i2c0, TCS34725_ADDRESS, buffer, 2, false);
    return (uint16_t)(buffer[1] << 8) | buffer[0];
}

/*
--- INICIALIZAR SENSOR TCS ---
    Inicializa o sensor com LED desligado.
*/
void tcs_init() {
    tcs_write8(ENABLE_REG, 0x00);         // Desativa tudo
    tcs_write8(ATIME_REG, 0xEB);          // Tempo de integração ~700ms
    tcs_write8(CONTROL_REG, 0x01);        // Ganho x4
}

/*
--- HABILITAR SENSOR TCS ---
    Liga o sensor e o LED interno.
*/
void tcs_enable() {
    tcs_write8(ENABLE_REG, 0x03);
}

/*
--- DESABILITAR SENSOR TCS ---
    Desliga o sensor e o LED interno.
*/
void tcs_disable() {
    tcs_write8(ENABLE_REG, 0x00);
}

/*
--- ALGORITMO DE INTERPRETAÇÃO DE COR ---
    A detecção de cor varia com parâmetros como a distância entre o
    sensor e a amostra, tal como a saturação e a reflexividade da cor.
    Para o algoritmo abaixo, realizei uma série de testes com algumas
    camisetas coloridas.

    OBS.: Como esperado, o sensor funciona melhor para detectar cores
    vibrantes.
*/
const char* detect_color(uint16_t r, uint16_t g, uint16_t b) {
    if (max(max(r, g), b) == r) {   // Maior valor é VERMELHO
        return "Vermelho";
    }
    if (max(max(r, g), b) == g) {   // Maior valor é VERDE
        if (max(r, b) == r) {               // Segundo maior valor é VERMELHO
            return "Amarelo";
        } else {                            // Segundo maior valor é AZUL
            if (b >= 90) {
                return "Ciano";
            }
            if ((b >= 80) && (r >= 60)) {
                return "Branco";
            }
            else {
                return "Verde";
            }
        }
    }
    if (max(max(r, g), b) == b) {   // Maior valor é AZUL
        if (max (r, g) == r) {              // Segundo maior valor é VERMELHO
            return "Magenta";
        }
        else {                              // Segundo maior valor é VERDE
            return "Azul";
        }
    }
    else {
        return "Desconhecido";
    }
}

/*
--- FUNÇÃO PRINCIPAL ---
*/
int main() {
    stdio_init_all();       // Inicialização geral
    config_i2c();           // Configuração de I2C

    sleep_ms(3000);
    printf("Inicializando sensor de cor...\n");     // Mensagem de inicialização

    tcs_init();             // Inicialização do sensor de cor
    sleep_ms(50);
    tcs_enable();           // Habilitação do sensor de cor
    sleep_ms(700);          // ON HOLD: Espera integração

    while (true) {
        // Leitura de dados
        uint16_t clear = tcs_read16(CDATAL);
        uint16_t red = tcs_read16(RDATAL);
        uint16_t green = tcs_read16(GDATAL);
        uint16_t blue = tcs_read16(BDATAL);

        // Evitar divisão por zero
        if (clear == 0) clear = 1;

        // Normalizar valores (0~255)
        uint16_t r = (red * 255) / clear;
        uint16_t g = (green * 255) / clear;
        uint16_t b = (blue * 255) / clear;

        // Interpretar valores lidos
        const char* color = detect_color(r, g, b);

        // Impressão de valores no terminal
        printf("Detectado: %s (R:%d G:%d B:%d)\n", color, r, g, b);

        sleep_ms(1000); // Espera entre leituras
    }

    return 0;
}
