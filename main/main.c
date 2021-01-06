#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_task.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "iot_device.h"

static const char *TAG = "main";

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  wifi_init_sta();

  iot_device_start();
}
