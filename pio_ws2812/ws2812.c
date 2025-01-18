/**
 * Código para controle de LEDs WS2812 na Raspberry Pi Pico.
 * Demonstra diferentes padrões de iluminação, utilizando o PIO (Programmable I/O) da Pico.
 * Baseado no SDK oficial da Raspberry Pi Pico.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

// Definições para LEDs RGB ou RGBW (True para RGBW, False para RGB)
#define IS_RGBW false
#define NUM_PIXELS 150 // Número de LEDs WS2812 conectados

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// Define o pino padrão como 7 caso o board não tenha definido outro WS2812 pin.
#define WS2812_PIN 7
#endif

// Verificação do suporte ao pino configurado
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Tentativa de usar um pino >= 32 em uma plataforma que não o suporta
#endif

/**
 * Função para enviar dados para o LED WS2812.
 * @param pio: Instância do PIO.
 * @param sm: Máquina de estado (state machine) do PIO.
 * @param pixel_grb: Cor no formato GRB (Green, Red, Blue).
 */
static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u); // Envia os dados para o LED
}

/**
 * Função para gerar cores RGB no formato GRB.
 * @param r: Intensidade do vermelho (0-255).
 * @param g: Intensidade do verde (0-255).
 * @param b: Intensidade do azul (0-255).
 * @return: Valor de cor no formato GRB.
 */
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

/**
 * Padrão: Snakes.
 * Cria uma sequência de cores em forma de "cobras" se movendo pelos LEDs.
 */
void pattern_snakes(PIO pio, uint sm, uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(pio, sm, urgb_u32(0xff, 0, 0)); // Vermelho
        else if (x >= 15 && x < 25)
            put_pixel(pio, sm, urgb_u32(0, 0xff, 0)); // Verde
        else if (x >= 30 && x < 40)
            put_pixel(pio, sm, urgb_u32(0, 0, 0xff)); // Azul
        else
            put_pixel(pio, sm, 0); // Apaga
    }
}

/**
 * Padrão: Dados aleatórios.
 * Preenche os LEDs com cores aleatórias.
 */
void pattern_random(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return; // Atualiza apenas em múltiplos de 8
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand()); // Cor aleatória
}

/**
 * Padrão: Sparkle.
 * LEDs piscam aleatoriamente criando um efeito de brilho.
 */
void pattern_sparkle(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand() % 16 ? 0 : 0xffffffff); // Sparkle aleatório
}

/**
 * Padrão: Gradiente de cinza.
 * Transita entre tons de cinza para os LEDs.
 */
void pattern_greys(PIO pio, uint sm, uint len, uint t) {
    uint max = 100; // Limite de intensidade para evitar consumo excessivo
    t %= max;
    for (uint i = 0; i < len; ++i) {
        put_pixel(pio, sm, t * 0x10101); // Gradiente de cinza
        if (++t >= max) t = 0;
    }
}

// Tabela de padrões disponíveis
typedef void (*pattern)(PIO pio, uint sm, uint len, uint t);
const struct {
    pattern pat;          // Ponteiro para função do padrão
    const char *name;     // Nome do padrão
} pattern_table[] = {
        {pattern_snakes,  "Snakes!"},
        {pattern_random,  "Random data"},
        {pattern_sparkle, "Sparkles"},
        {pattern_greys,   "Greys"},
};

/**
 * Função principal do programa.
 * Inicializa o PIO, configura o programa WS2812 e executa padrões de LEDs.
 */
int main() {
    stdio_init_all();
    printf("Teste de LEDs WS2812, usando o pino %d\n", WS2812_PIN);

    PIO pio;
    uint sm;
    uint offset;

    // Inicializa e carrega o programa WS2812 no PIO
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true
    );
    hard_assert(success); // Assegura que o programa foi carregado com sucesso

    // Configura a máquina de estado para controlar os LEDs WS2812
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;
    while (1) {
        // Seleciona um padrão aleatório da tabela
        int pat = rand() % count_of(pattern_table);
        int dir = (rand() >> 30) & 1 ? 1 : -1; // Direção: 1 (frente) ou -1 (trás)
        puts(pattern_table[pat].name); // Imprime o nome do padrão selecionado
        puts(dir == 1 ? "(forward)" : "(backward)");

        // Executa o padrão selecionado
        for (int i = 0; i < 1000; ++i) {
            pattern_table[pat].pat(pio, sm, NUM_PIXELS, t);
            sleep_ms(10); // Pequeno atraso entre atualizações
            t += dir;     // Incrementa ou decrementa t conforme a direção
        }
    }

    // Remove o programa do PIO e libera recursos (nunca alcançado nesse código)
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}

