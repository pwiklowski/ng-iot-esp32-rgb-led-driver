#include "cJSON.h"
#include "iot.h"
#include "led.h"
#include <stdio.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "led.h"

char* TAG = "IOT_DEVICE";

char* DEVICE_NAME = "Smart RGB Light";
char* DEVICE_UUID = "d60a0a26-0876-40b4-b336-8a36f879112e";
char* VARIABLE_UUID = "56be315a-05b2-4f5e-8fd1-342b40c006fe";

#define DEVICE_DESCRIPTION "{\"type\":0,\"reqId\":0,\"args\":{\"config\":{\"name\":\"%s\",\"deviceUuid\":\"%s\",\"vars\":{\"%s\":{\
\"name\":\"color\",\"schema\":{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"type\":\"object\",\
\"properties\":{\"red\":{\"type\":\"integer\"}, \"green\":{\"type\":\"integer\"}, \"blue\":{\"type\":\"integer\"}},\"required\":[\"red\", \"green\", \"blue\"],\"additionalProperties\":false},\"access\":\"rw\",\
\"value\":{\"red\":0, \"green\":0, \"blue\":0}}}}}}"

#define NOTIFY_TEMPLATE "{\"type\":6,\"args\":{\"deviceUuid\":\"%s\",\"variableUuid\":\"%s\",\"value\":{\"red\":%d, \"green\":%d, \"blue\":%d}}}"

char *description = NULL;

//make sure to call it only once or make iot-device_init / deinit functions
char *iot_device_get_description() {
  return description;
}

void iot_device_value_updated(uint8_t red, uint8_t green, uint8_t blue) {
  char* notification[200];
  uint16_t len = sprintf(notification, NOTIFY_TEMPLATE, DEVICE_UUID, VARIABLE_UUID, red, green, blue);
  iot_emit_event(MSG_IOT_VALUE_UPDATED, notification, len);
}

void iot_device_event_handler(const char *payload, const size_t len) {
  ESP_LOGI(TAG, "iot_event_handler %s", payload);

  cJSON *json = cJSON_Parse((char*)payload);
  int event_type = cJSON_GetObjectItemCaseSensitive(json, "type")->valueint;

  if (event_type == SetValue) {
    cJSON *args = cJSON_GetObjectItemCaseSensitive(json, "args");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(args, "value");
    cJSON *red = cJSON_GetObjectItemCaseSensitive(value, "red");
    cJSON *green = cJSON_GetObjectItemCaseSensitive(value, "green");
    cJSON *blue = cJSON_GetObjectItemCaseSensitive(value, "blue");
    led_set_rgb(red->valueint, green->valueint, blue->valueint);
    iot_device_value_updated(red->valueint, green->valueint, blue->valueint);
  }
}

void iot_device_init() {
  size_t needed = snprintf(NULL, 0, DEVICE_DESCRIPTION, DEVICE_NAME, DEVICE_UUID, VARIABLE_UUID) + 1;
  description = malloc(needed);
  sprintf(description, DEVICE_DESCRIPTION, DEVICE_NAME, DEVICE_UUID, VARIABLE_UUID);

  led_init();
}

void iot_device_deinit() {
  free(description);
}