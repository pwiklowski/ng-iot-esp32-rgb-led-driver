#include "iot_connection.h"

#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_websocket_client.h"

#include "cJSON.h"

#include "led.h"

static const char *TAG = "WEBSOCKET";

esp_websocket_client_handle_t client;
#define DEVICE_NAME  "Smart RGB Light"
#define DEVICE_UUID "d60a0a26-0876-40b4-b336-8a36f879112e"
#define VARIABLE_UUID "56be315a-05b2-4f5e-8fd1-342b40c006fe"

#define DEVICE_DESCRIPTION "{\"type\":0,\"reqId\":0,\"args\":{\"config\":{\"name\":\"" DEVICE_NAME \
"\",\"deviceUuid\":\"" DEVICE_UUID "\",\"vars\":{\"" VARIABLE_UUID "\":{\
\"name\":\"color\",\
\"schema\":{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"type\":\"object\",\
\"properties\":{\"red\":{\"type\":\"integer\"}, \"green\":{\"type\":\"integer\"}, \"blue\":{\"type\":\"integer\"}},\"required\":[\"red\", \"green\", \"blue\"],\"additionalProperties\":false},\"access\":\"rw\",\
\"value\":{\"red\":0, \"green\":0, \"blue\":0}}}}}}"

#define NOTIFY_TEMPLATE                                                             \
"{\"type\":6,\"args\":{\"deviceUuid\":\"" DEVICE_UUID                        \
"\",\"variableUuid\":\"" VARIABLE_UUID                                       \
"\",\"value\":{\"red\":%d, \"green\":%d, \"blue\":%d}}}"

void on_connected() {
  ESP_LOGI(TAG, "send device description %s", DEVICE_DESCRIPTION);
  esp_websocket_client_send(client, DEVICE_DESCRIPTION, strlen(DEVICE_DESCRIPTION), portMAX_DELAY);
}

void notify_change(uint8_t red, uint8_t green, uint8_t blue) {
  char notification[200];
  uint8_t len = sprintf(notification, NOTIFY_TEMPLATE, red, green, blue);
  esp_websocket_client_send(client,notification, len, portMAX_DELAY);
}

void parse_message(const char *payload, const size_t len)
{
  cJSON *json = cJSON_Parse((char*)payload);

  int event_type = cJSON_GetObjectItemCaseSensitive(json, "type")->valueint;

  if (event_type == SetValue) {
    cJSON *args = cJSON_GetObjectItemCaseSensitive(json, "args");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(args, "value");
    cJSON *red = cJSON_GetObjectItemCaseSensitive(value, "red");
    cJSON *green = cJSON_GetObjectItemCaseSensitive(value, "green");
    cJSON *blue = cJSON_GetObjectItemCaseSensitive(value, "blue");
    led_set_rgb(red->valueint, green->valueint, blue->valueint);
    notify_change(red->valueint, green->valueint, blue->valueint);
  }
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base,
                                    int32_t event_id, void *event_data) {
  esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
  switch (event_id) {
  case WEBSOCKET_EVENT_CONNECTED:
    ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
    on_connected();
    break;
  case WEBSOCKET_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
    break;
  case WEBSOCKET_EVENT_DATA:
    if (data->op_code == 1) { //text frame
      ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
      ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d", data->payload_len, data->data_len, data->payload_offset);
      parse_message(data->data_ptr, data->data_len);
    }
    break;
  case WEBSOCKET_EVENT_ERROR:
    ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
    break;
  }
}

void websocket_app_start(void) {
  esp_websocket_client_config_t websocket_cfg = {
      .uri = "wss://iot.wiklosoft.com/connect/device"};

  ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

  client = esp_websocket_client_init(&websocket_cfg);
  esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

  esp_websocket_client_start(client);
}
void websocket_app_stop(void) {
  esp_websocket_client_stop(client);
  ESP_LOGI(TAG, "Websocket Stopped");
  esp_websocket_client_destroy(client);
}
