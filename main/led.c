
#include "driver/gpio.h"
#include "driver/ledc.h"

ledc_channel_config_t led_red_channel = {
  .channel = LEDC_CHANNEL_0,
  .duty = 0,
  .gpio_num = 33,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

ledc_channel_config_t led_green_channel = {.channel = LEDC_CHANNEL_1,
  .duty = 0,
  .gpio_num = 27,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

ledc_channel_config_t led_blue_channel = {
  .channel = LEDC_CHANNEL_2,
  .duty = 0,
  .gpio_num = 22,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

void led_init() {
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
      .freq_hz = 400,                      // frequency of PWM signal
      .speed_mode = LEDC_HIGH_SPEED_MODE,  // timer mode
      .timer_num = LEDC_TIMER_0,           // timer index
      .clk_cfg = LEDC_AUTO_CLK,            // Auto select the source clock
  };
  ledc_timer_config(&ledc_timer);

  ledc_channel_config(&led_red_channel);
  ledc_channel_config(&led_green_channel);
  ledc_channel_config(&led_blue_channel);
}

void led_set_value(ledc_channel_config_t channel, uint8_t duty) {
  ledc_set_duty(channel.speed_mode, channel.channel, duty);
  ledc_update_duty(channel.speed_mode, channel.channel);
}

void led_set_rgb(uint8_t red, uint8_t green, uint8_t blue) {
  led_set_value(led_red_channel, red);
  led_set_value(led_green_channel, green);
  led_set_value(led_blue_channel, blue);
}
