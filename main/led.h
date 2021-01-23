#ifndef __LED_H__
#define __LED_H__

#include "driver/gpio.h"
#include "driver/ledc.h"
#include <math.h>

#define LED_PWM_RESOLUTION LEDC_TIMER_8_BIT
#define LED_MAX_VALUE (pow(2, LED_PWM_RESOLUTION)-1)

void led_init();
void led_set_rgb(uint8_t red, uint8_t green, uint8_t blue);
void led_set_value(ledc_channel_config_t channel, uint8_t duty);

#endif // __LED_H__invalid operands to binary ^ (have 'int' and 'double')
