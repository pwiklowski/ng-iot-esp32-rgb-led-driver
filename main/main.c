/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "main.h"

#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "string.h"

#include "led.h"
#include "wifi.h"

static const char *TAG = "main";

Config_t config;

void read_config() {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    printf("Reading from NVS ... ");

    memset((uint8_t*)&config, 0, sizeof(Config_t));
    config.access_token_length = TOKEN_MAX_LEN;
    err = nvs_get_str(my_handle, "access_token", config.access_token, &config.access_token_length);
    config.refresh_token_length = REFRESH_TOKEN_MAX_LEN;
    err = nvs_get_str(my_handle, "refresh_token", config.refresh_token, &config.refresh_token_length);

    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    nvs_close(my_handle);
  }
}

void save_config() {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    err = nvs_set_str(my_handle, "access_token", config.access_token);
    err = nvs_set_str(my_handle, "refresh_token", config.refresh_token);

    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(my_handle);
  }
}

void app_main(void) {
  printf("Hello world!\n");
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  read_config();

  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
  wifi_init_sta();

  led_init();
}
