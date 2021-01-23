#include "led.h"
#include "iot_device.h"


#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB)

ledc_channel_config_t led_channel_6 = {
  .channel = LEDC_CHANNEL_0,
  .duty = 0,
  .gpio_num = 15,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

ledc_channel_config_t led_channel_5 = {
  .channel = LEDC_CHANNEL_1,
  .duty = 0,
  .gpio_num = 13,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

ledc_channel_config_t led_channel_4 = {
  .channel = LEDC_CHANNEL_2,
  .duty = 0,
  .gpio_num = 2,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

#endif

ledc_channel_config_t led_channel_1 = {
  .channel = LEDC_CHANNEL_3,
  .duty = 0,
  .gpio_num = 22,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};


ledc_channel_config_t led_channel_2 = {
  .channel = LEDC_CHANNEL_4,
  .duty = 0,
  .gpio_num = 19,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};


ledc_channel_config_t led_channel_3 = {
  .channel = LEDC_CHANNEL_5,
  .duty = 0,
  .gpio_num = 23,
  .speed_mode = LEDC_HIGH_SPEED_MODE,
  .hpoint = 0,
  .timer_sel = LEDC_TIMER_0
};

void led_init() {
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LED_PWM_RESOLUTION, // resolution of PWM duty
      .freq_hz = 400,                      // frequency of PWM signal
      .speed_mode = LEDC_HIGH_SPEED_MODE,  // timer mode
      .timer_num = LEDC_TIMER_0,           // timer index
      .clk_cfg = LEDC_AUTO_CLK,            // Auto select the source clock
  };
  ledc_timer_config(&ledc_timer);

#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB)
  ledc_channel_config(&led_channel_6);
  ledc_channel_config(&led_channel_5);
  ledc_channel_config(&led_channel_4);
#endif

  ledc_channel_config(&led_channel_1);
  ledc_channel_config(&led_channel_2);
  ledc_channel_config(&led_channel_3);
}

void led_set_value(ledc_channel_config_t channel, uint8_t duty) {
  ledc_set_duty(channel.speed_mode, channel.channel, duty);
  ledc_update_duty(channel.speed_mode, channel.channel);
}

void led_set_rgb(uint8_t red, uint8_t green, uint8_t blue) {

#if  defined(VARIANT_6CH_RGB)
  led_set_value(led_channel_6, red);
  led_set_value(led_channel_5, green);
  led_set_value(led_channel_4, blue);
#endif

#if  defined(VARIANT_3CH_RGB)
  led_set_value(led_channel_3, red);
  led_set_value(led_channel_2, green);
  led_set_value(led_channel_1, blue);

#endif
}
