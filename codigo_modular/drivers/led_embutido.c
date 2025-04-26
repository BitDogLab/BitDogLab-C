#include "led_embutido.h"
#include "pico/cyw43_arch.h"

void led_hw_init() {
    cyw43_arch_init();
}

void led_hw_on() {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
}

void led_hw_off() {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}
