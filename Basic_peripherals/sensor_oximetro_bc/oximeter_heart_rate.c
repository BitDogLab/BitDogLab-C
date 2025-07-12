/*
SENSOR DE BATIMENTOS CARDÍACOS E OXÍMETRO

O código abaixo foi desenvolvido com propósitos didáticos, para uso
ao longo do Curso de Capacitação em Sistemas Embarcados - Embarcatech.

Para usar o código abaixo, conecte o módulo MAX30102 (OXI BAT),
usando um conector JST SH de 4 fios, ao port I2C 0 da BitDogLab.

A frequência cardíaca (em BPM) e o valor de SpO2 (oxigenação, em %)
podem ser lidos via Serial Monitor.
*/

// AVISO:
// O código abaixo foi feito para operação com o módulo MAX30102.
// Portanto, pode não ser compatível com um módulo MAX30101.

// Bibliotecas inclusas
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições de I2C
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define MAX30102_ADDR   0x57

// Lista de endereços de registradores
#define REG_INTR_STATUS_1   0x00
#define REG_INTR_ENABLE_1   0x02
#define REG_FIFO_WR_PTR     0x04
#define REG_FIFO_OVF_CNT    0x05
#define REG_FIFO_RD_PTR     0x06
#define REG_FIFO_DATA       0x07
#define REG_FIFO_CONFIG     0x08
#define REG_MODE_CONFIG     0x09
#define REG_SPO2_CONFIG     0x0A
#define REG_LED1_PA         0x0C    // Corrente do LED RED
#define REG_LED2_PA         0x0D    // Corrente do LED IR
#define REG_PART_ID         0xFF    // Deve retornar 0x15

/*
--- CONFIGURAR I2C ---
    Função de inicialização e configuração geral do I2C.
*/
void config_i2c() {
    i2c_init(i2c0, 400 * 1000); // Comunicação I2C a 400 kHz (modo Fast) para melhor desempenho
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

/*
--- ESCRITA DO SENSOR MAX30102 ---
    Escreve um valor de 8 bits em um registrador do sensor, montando um
    buffer com o endereço do registrador e o valor a ser escrito. Usa
    i2c_write_blocking() para transmitir ao sensor via I2C.
*/
static inline void max30102_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(I2C_PORT, MAX30102_ADDR, buf, 2, false); // Libera escrita
}

/*
--- LEITURA DO SENSOR MAX30102 ---
    Lê um valor de 8 bits de um registrador do sensor, primeiro enviando
    o endereço do registrador com repeated start (não finaliza a
    comunicação com stop bit), depois lê 1 byte.
*/
static inline uint8_t max30102_read(uint8_t reg) {
    uint8_t val;
    i2c_write_blocking(I2C_PORT, MAX30102_ADDR, &reg, 1, true); // Bloqueia escrita
    i2c_read_blocking(I2C_PORT, MAX30102_ADDR, &val, 1, false); // Libera leitura
    return val;
}

/*
--- INICIALIZAÇÃO DO SENSOR ---
*/
void max30102_init(void) {
    // Soft reset
    max30102_write(REG_MODE_CONFIG, 0x40);
    sleep_ms(100);

    // Limpar ponteiros do buffer FIFO
    max30102_write(REG_FIFO_WR_PTR, 0x00);  // Ponteiro Write
    max30102_write(REG_FIFO_OVF_CNT, 0x00); // Ponteiro Overflow
    max30102_write(REG_FIFO_RD_PTR, 0x00);  // Ponteiro Read

    // Configuração FIFO:
    //   - média de amostras = 4
    //   - rollover habilitado
    //   - limite de amostras = 17
    max30102_write(REG_FIFO_CONFIG, (0b010 << 5) | (1 << 4) | 0x11);

    // Configuração de SpO2:
    //   - resolução do ADC: 4096 nA
    //   - taxa de amostragem de 100 Hz
    //   - 411 µs (18 bits de amplitude)
    max30102_write(REG_SPO2_CONFIG, (0b01 << 5) | (0b011 << 2) | 0b11);

    // Amplitudes de operação dos LEDs (~6.4 mA -> 0x24)
    max30102_write(REG_LED1_PA, 0x24);  // RED
    max30102_write(REG_LED2_PA, 0x24);  // IR

    // Habilitação do modo SpO2 (RED e IR)
    //   - 0x03 = modo SpO₂
    max30102_write(REG_MODE_CONFIG, 0x03);
}

/*
--- LEITURA DE AMOSTRA RED/IR ---
    1. Verifica se há dados não lidos no buffer FIFO;
    2. Caso haja, lê 6 bytes do buffer (3 para RED e 3 para IR);
    3. Constrói inteiros de 18 bits.
*/
bool max30102_read_sample(uint32_t *red, uint32_t *ir) {
    uint8_t wr = max30102_read(REG_FIFO_WR_PTR);
    uint8_t rd = max30102_read(REG_FIFO_RD_PTR);
    if (wr == rd) return false;     // Sem amostras não lidas

    uint8_t data[6];
    uint8_t reg = REG_FIFO_DATA;
    i2c_write_blocking(I2C_PORT, MAX30102_ADDR, &reg, 1, true);
    i2c_read_blocking (I2C_PORT, MAX30102_ADDR, data, 6, false);

    // 3 bytes por amostra -> Mascarar para 18 bits
    *red = ((uint32_t)data[0] << 16 | data[1] << 8 | data[2]) & 0x3FFFF;
    *ir  = ((uint32_t)data[3] << 16 | data[4] << 8 | data[5]) & 0x3FFFF;
    return true;        // Amostras não lidas (há dados em standby)
}

#define SAMPLE_SIZE 100             // 100 amostras para RED e IR
uint32_t red_buffer[SAMPLE_SIZE];   // Buffer de RED
uint32_t ir_buffer[SAMPLE_SIZE];    // Buffer de IR

/*
--- CÁLCULO DE OXIGENAÇÃO (SPO2) ---
    Com base nos valores de RED e IR, estima a oxigenação no sangue.
*/
float calculate_spo2(uint32_t *red, uint32_t *ir)
{
    float red_ac = 0, ir_ac = 0;
    float red_dc = 0, ir_dc = 0;

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        red_dc += red[i];
        ir_dc  += ir[i];
    }

    red_dc /= SAMPLE_SIZE;
    ir_dc  /= SAMPLE_SIZE;

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        red_ac += fabsf(red[i] - red_dc);
        ir_ac  += fabsf(ir[i] - ir_dc);
    }

    red_ac /= SAMPLE_SIZE;
    ir_ac  /= SAMPLE_SIZE;

    float ratio = (red_ac / red_dc) / (ir_ac / ir_dc);
    float spo2 = 110.0f - 25.0f * ratio; // Fórmula empírica

    if (spo2 > 100.0f) spo2 = 100.0f;
    if (spo2 < 0.0f)   spo2 = 0.0f;
    return spo2;
}

/*
--- CÁLCULO DE FREQUÊNCIA CARDÍACA ---
    Com base nos valores de IR, estima a frequência cardíaca.
*/
float calculate_bpm(uint32_t *ir, float sample_rate_hz)
{
    int peak_count = 0;
    for (int i = 1; i < SAMPLE_SIZE - 1; i++) {
        if (ir[i] > ir[i - 1] && ir[i] > ir[i + 1] && ir[i] > 10000)
            peak_count++;
    }

    float duration_sec = SAMPLE_SIZE / sample_rate_hz;
    return (peak_count / (duration_sec * 0.5f));  // Estimativa com aproximação de valor final
}

/*
--- FUNÇÃO PRINCIPAL ---
*/
int main(void)
{
    stdio_init_all();   // Inicialização geral
    config_i2c();       // Configuração de I2C

    max30102_init();    // Inicialização do sensor
    sleep_ms(3000);
    uint8_t part_id = max30102_read(REG_PART_ID);
    if (part_id == 0x15) {
        printf("MAX30102 pronto (Part ID: 0x%02X)\n", part_id);
    } else {
        printf("Erro: Part ID inesperado (0x%02X). Verifique conexão ou compatibilidade.\n", part_id);
    }

    const float sample_rate = 100.0f;

    while (true)
    {
        // Leitura de dados
        int samples_collected = 0;
        while (samples_collected < SAMPLE_SIZE)
        {
            uint32_t red, ir;
            if (max30102_read_sample(&red, &ir))
            {
                red_buffer[samples_collected] = red;
                ir_buffer[samples_collected] = ir;
                samples_collected++;
            }
            sleep_ms(10); // ~100 Hz -> freq. de leitura bate com a taxa de amostragem
        }

        float bpm = calculate_bpm(ir_buffer, sample_rate);
        float spo2 = calculate_spo2(red_buffer, ir_buffer);

        printf("Heart Rate: %.1f BPM\tSpO2: %.1f%%\n", bpm, spo2);
    }
}
