#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "hardware/gpio.h"
#include "hardware/pwm.h"

/**
 * PINOUT MOTOR DRIVER
 * STDBY: GP20
 * LEFT:
 *  IN1: GP04 // Forward!
 *  IN2: GP09 // Backward!
 *  PWM: GP08
 * RIGHT:
 *  IN1: GP18 // Forward!
 *  IN2: GP19 // Backward!
 *  PWM: GP16
 */

#define MOTOR_STDBY           20
#define MOTOR_LEFT_FORWARD    4
#define MOTOR_LEFT_BACKWARD   9
#define MOTOR_LEFT_SIGNAL     8
#define MOTOR_RIGHT_FORWARD   18
#define MOTOR_RIGHT_BACKWARD  19
#define MOTOR_RIGHT_SIGNAL    16

extern void motor_setup();
extern void motor_set_right_level(uint16_t level, bool forward);
extern void motor_set_left_level(uint16_t level, bool forward);

#define motor_disable() gpio_put(MOTOR_STDBY, false)
#define motor_enable() gpio_put(MOTOR_STDBY, true)

extern void motor_set_both_level(uint16_t level, bool forward);

#endif