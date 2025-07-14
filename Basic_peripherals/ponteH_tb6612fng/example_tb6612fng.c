#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "motor.h"
#include <math.h>

#define MOTOR_1    8
#define MOTOR_2    16

void h_bridge_init(void);
void pwm_setup_manual(uint pwm_pin);
void pwm_setup_auto(uint pwm_pin);
void example1();
void example2();
void set_motor_from_float(float level_float);
void example3();

int main() {
    stdio_init_all();
    example1();
    //example2();
    //example3();
    while (1) tight_loop_contents();
}

void example1() {
    h_bridge_init();

    pwm_setup_manual(MOTOR_1);
    pwm_setup_manual(MOTOR_2);
    //pwm_setup_auto(MOTOR_1);
    //pwm_setup_auto(MOTOR_2);

    gpio_put(4, 1); gpio_put(9, 0);  // left motor forward
    gpio_put(18, 1); gpio_put(19, 0); // right motor forward

    pwm_set_gpio_level(MOTOR_1, (uint16_t)(0.35f*255));  // 35% duty cycle
    pwm_set_gpio_level(MOTOR_2, (uint16_t)(0.45f*255));

    sleep_ms(1500); // run motors

    pwm_set_gpio_level(MOTOR_1, 0); pwm_set_gpio_level(MOTOR_2, 0); // stop
}

void h_bridge_init(void){
    gpio_init(20); gpio_set_dir(20, 1); gpio_put(20, 1); // standby high

    gpio_init(4); gpio_set_dir(4, 1); gpio_init(9); gpio_set_dir(9, 1); // pins that constrols direction motor 1
    gpio_init(18); gpio_set_dir(18, 1); gpio_init(19); gpio_set_dir(19, 1); // pins that constrols direction motor 2
}

void pwm_setup_manual(uint pwm_pin){
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pwm_pin);

    pwm_set_clkdiv(slice, 4.0f); // set clock divider
    pwm_set_wrap(slice, 255); // set wrap value (top), can be 65536
    pwm_set_enabled(slice, true);
}
void pwm_setup_auto(uint pwm_pin){
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pwm_pin);

    pwm_config config = pwm_get_default_config(); // default wrap is 0xffff
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_config_set_wrap(&config, 255);
    pwm_init(slice, &config, true); // true means enabled
}

void example2() {
    motor_setup();
    motor_enable();

    float level_float = -0.35f * 65535; // power level
    
    bool forward = level_float > 0; // obtem direcao
    float magnitude = fabsf(level_float); // obtem modulo do sinal
    float limitado = fminf(magnitude, 65535.0f); // limita a um maximo
    uint16_t level = (uint16_t)limitado; // converte o tipo

    motor_set_both_level(level, forward);
    sleep_ms(1500);
    motor_set_both_level(0, forward);
}

void example3() {
    motor_setup();
    motor_enable();

    float level_float = -0.35f * 255;
    set_motor_from_float(level_float);
    sleep_ms(1500);
    set_motor_from_float(0.0f);
}

void set_motor_from_float(float level_float) {
    bool direction = level_float > 0;

    // abs, limita, cast, bitshift
    uint16_t level = (uint16_t)fminf(fabsf(level_float), 255.0f) << 8;

    motor_set_both_level(level, direction);
}