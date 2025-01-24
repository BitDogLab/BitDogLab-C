#include <stdio.h>
#include "pico/stdlib.h"

#define ROWS 4
#define COLS 4

const uint row_pins[ROWS] = {18, 19, 16, 17}; // Pinos conectados nas linhas R1, R2, R3, R4
const uint col_pins[COLS] = {20, 4, 9, 8}; // Pinos conectados nas linhas C1, C2, C3, C4

const char key_map[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void keypad_init() {
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 0); // configura as linhas como saídas em nível baixo
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_down(col_pins[i]); // configura as colunas como entradas com pull-down
    }
}

char read_keypad() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 1); // Ativa a linha atual
        for (int col = 0; col < COLS; col++) {
            if (gpio_get(col_pins[col])) { // Verifica se a coluna atual está em nível alto
                gpio_put(row_pins[row], 0); // Desativa a linha atual
                return key_map[row][col]; // Retorna o caractere correspondente
            }
        }
        gpio_put(row_pins[row], 0); // configura a linha como saída em nível baixo
    }
    return '\0'; // Retorna nulo se nenhuma tecla foi pressionada
}

int main()
{
    stdio_init_all();
    keypad_init(); // Inicializa o teclado matricial

    while (true) {
        char key = read_keypad(); // Lê a tecla pressionada
        if (key != '\0') { // Verifica se alguma tecla foi pressionada
            printf("Tecla pressionada: %c\n", key); // Exibe a tecla pressionada
            sleep_ms(350); // Aguarda 350 ms para evitar múltiplas leituras
        }
    }
    return 0;
}
