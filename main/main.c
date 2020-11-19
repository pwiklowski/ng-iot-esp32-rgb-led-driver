/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "wifi.h"

#define LEDC_HS_CH0_GPIO (33)
#define LEDC_HS_CH0_CHANNEL LEDC_CHANNEL_0

#define LEDC_TEST_FADE_TIME (3000)
#define LEDC_TEST_DUTY (4000)

static const char *TAG = "main";

void app_main(void) {
  printf("Hello world!\n");
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();
  int ch;

  /*
   * Prepare and set configuration of timers
   * that will be used by LED Controller
   */
  ledc_timer_config_t ledc_timer = {
      .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
      .freq_hz = 5000,                      // frequency of PWM signal
      .speed_mode = LEDC_HIGH_SPEED_MODE,   // timer mode
      .timer_num = LEDC_TIMER_0,            // timer index
      .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
  };
  ledc_timer_config(&ledc_timer);

  /*
   * Prepare individual configuration
   * for each channel of LED Controller
   * by selecting:
   * - controller's channel number
   * - output duty cycle, set initially to 0
   * - GPIO number where LED is connected to
   * - speed mode, either high or low
   * - timer servicing selected channel
   *   Note: if different channels use one timer,
   *         then frequency and bit_num of these channels
   *         will be the same
   */

  ledc_channel_config_t ledc_channel = {.channel = LEDC_HS_CH0_CHANNEL,
                                        .duty = 0,
                                        .gpio_num = LEDC_HS_CH0_GPIO,
                                        .speed_mode = LEDC_HIGH_SPEED_MODE,
                                        .hpoint = 0,
                                        .timer_sel = LEDC_TIMER_0};

  ledc_channel_config(&ledc_channel);

  // Initialize fade service.
  ledc_fade_func_install(0);

  while (1) {
    ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel,
                            LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel,
                    LEDC_FADE_NO_WAIT);
    vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

    ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 0,
                            LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel,
                    LEDC_FADE_NO_WAIT);
    vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);
  }
}
