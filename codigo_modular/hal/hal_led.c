#include "hal_led.h"
#include "led_embutido.h"
#include "pico/stdlib.h"

void hal_led_init() {
    led_hw_init();
}

void hal_led_toggle() {
    led_hw_on();
    sleep_ms(500);
    led_hw_off();
    sleep_ms(500);
}
