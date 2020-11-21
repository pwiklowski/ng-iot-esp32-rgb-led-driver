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



static const char *TAG = "WEBSOCKET";

esp_websocket_client_handle_t client;
#define DEVICE_NAME  "Smart Light 4"
#define DEVICE_UUID "02fd0148-7023-4e36-a6a5-79ae12753d94"
#define VARIABLE_UUID "c391b0c7-0464-4a8d-aee8-0fe307a85247"

#define DEVICE_DESCRIPTION "{\"type\":0,\"reqId\":0,\"args\":{\"config\":{\"name\":\"" DEVICE_NAME \
      "\",\"deviceUuid\":\"" DEVICE_UUID "\",\"vars\":{\"" VARIABLE_UUID "\":{\
       \"name\":\"relayState\",\
       \"schema\":{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"type\":\"object\",\
       \"properties\":{\"state\":{\"type\":\"boolean\"}},\"required\":[\"state\"],\"additionalProperties\":false}\",\"access\":\"rw\",\
       \"value\":{\"state\":false}}}}}}"

#define NOTIFY_ON "{\"type\":6,\"args\":{\"deviceUuid\":\"" DEVICE_UUID "\",\"variableUuid\":\"" VARIABLE_UUID "\",\"value\":{\"state\":true}}}"
#define NOTIFY_OFF "{\"type\":6,\"args\":{\"deviceUuid\":\"" DEVICE_UUID "\",\"variableUuid\":\"" VARIABLE_UUID "\",\"value\":{\"state\":false}}}"

void on_connected() {
  esp_websocket_client_send(client, DEVICE_DESCRIPTION, strlen(DEVICE_DESCRIPTION), portMAX_DELAY);
}

void notify_change(bool val) {
  if (val) {
    esp_websocket_client_send(client, NOTIFY_ON, strlen(NOTIFY_ON), portMAX_DELAY);
  } else {
    esp_websocket_client_send(client, NOTIFY_OFF, strlen(NOTIFY_OFF), portMAX_DELAY);
  }
}

void parse_message(const char *payload, const size_t len)
{
  cJSON *json = cJSON_Parse((char*)payload);

  int event_type = cJSON_GetObjectItemCaseSensitive(json, "type")->valueint;

  if (event_type == SetValue) {
    cJSON *args = cJSON_GetObjectItemCaseSensitive(json, "args");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(args, "value");
    cJSON *state= cJSON_GetObjectItemCaseSensitive(value, "state");
    ESP_LOGI(TAG, "set value %d!\n", state->valueint);
    notify_change(state->valueint);
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
