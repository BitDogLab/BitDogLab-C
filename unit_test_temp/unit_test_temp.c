// Implemente uma função de teste unitário em C para verificar se uma função de cálculo de temperatura retorna 
// corretamente valores em graus Celsius.

#include <stdio.h>
#include "pico/stdlib.h"
// https://www.throwtheswitch.org/unity
#include "unity.h"

void setUp(void) {
    // Configuração inicial, se necessário
}

void tearDown(void) {
    // Limpeza, se necessário
}

float adc_to_celsius(uint16_t raw_adc) {
    const float conversion_factor = 3.3f / 4095;
    float voltage = raw_adc * conversion_factor;
    return 27 - (voltage - 0.706f) / 0.001721f;
}

void test_adc_to_celsius_should_return_27_when_voltage_is_0_706() {
    uint16_t raw_adc = (uint16_t)(0.706f / (3.3f / 4095));  // Mock - Inverso da conversão
    float temp = adc_to_celsius(raw_adc);
    TEST_ASSERT_FLOAT_WITHIN(0.1, 27.0f, temp);
}

int main(void) {
    stdio_init_all(); // Inicializa a comunicação serial
    sleep_ms(3000); // Aguarda a USB conectar (opcional, mas útil)

    while (true) {
        printf("Iniciando testes Unity...\n");

        UNITY_BEGIN();
        RUN_TEST(test_adc_to_celsius_should_return_27_when_voltage_is_0_706);
        UNITY_END();

        printf("Testes finalizados. Reiniciando em 5 segundos...\n---\n");
        sleep_ms(5000);
    }
    
    return 0;
}
