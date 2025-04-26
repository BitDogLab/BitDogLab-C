// https://datasheets.raspberrypi.com/pico/Pico-R3-A4-Pinout.pdf
// Enunciado:
// A seguir, você encontra um código que inicializa o LED embutido da Raspberry Pi Pico W 
// e o faz piscar indefinidamente:

// #include "pico/stdlib.h"
// #include "pico/cyw43_arch.h"

// int main() {
//     stdio_init_all();
//     if (cyw43_arch_init()) {
//         return -1;
//     }

//     while (true) {
//         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
//         sleep_ms(500);
//         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
//         sleep_ms(500);
//     }
// }

#include "pico/stdlib.h"
#include "hal_led.h"

int main() {
    hal_led_init();

    while (true) {
        hal_led_toggle();
    }
}
