#include "cJSON.h"
#include "iot.h"
#include "led.h"
#include <stdio.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "led.h"
#include <string.h>

char *TAG = "IOT_DEVICE";

char *VARIABLE_UUID = "56be315a-05b2-4f5e-8fd1-342b40c006fe";

#define SCHEMA "{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"type\":\"object\",\"properties\":{\"red\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255}, \"green\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255}, \"blue\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255},\"power\":{\"type\":\"number\",\"minimum\":0,\"maximum\":1}},\"required\":[\"red\", \"green\", \"blue\",\"power\"],\"additionalProperties\":false}"

char device_uuid[38];
cJSON *color_value;
cJSON *color_value_red;
cJSON *color_value_green;
cJSON *color_value_blue;
cJSON *color_value_power;


cJSON* iot_device_get_description() {
  char version[32];
  char name[32];
  iot_get_app_version(name, version);

  cJSON *device = cJSON_CreateObject();
  cJSON_AddStringToObject(device, "name", name);
  cJSON_AddStringToObject(device, "deviceUuid", device_uuid);
  cJSON_AddStringToObject(device, "version", version);
  cJSON *vars = cJSON_AddObjectToObject(device, "vars");

  iot_create_variable_description(vars, VARIABLE_UUID, "color", "rw", SCHEMA, cJSON_Duplicate(color_value, true));
  return device;
}


void iot_device_value_updated(uint8_t red, uint8_t green, uint8_t blue, float power) {
  ESP_LOGI(TAG, "iot_device_value_updated %f %d %d %d", power, red, green, blue);

  cJSON_SetNumberValue(color_value_power, power);
  cJSON_SetNumberValue(color_value_red, red);
  cJSON_SetNumberValue(color_value_green, green);
  cJSON_SetNumberValue(color_value_blue, blue);

  iot_send_value_changed_notifcation(device_uuid, VARIABLE_UUID, color_value);
}

void iot_device_event_handler(const char *payload, const size_t len) {
  cJSON *json = cJSON_Parse((char*) payload);
  int event_type = cJSON_GetObjectItemCaseSensitive(json, "type")->valueint;

  if (event_type == SetValue) {
    cJSON *args = cJSON_GetObjectItemCaseSensitive(json, "args");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(args, "value");
    cJSON *red = cJSON_GetObjectItemCaseSensitive(value, "red");
    cJSON *green = cJSON_GetObjectItemCaseSensitive(value, "green");
    cJSON *blue = cJSON_GetObjectItemCaseSensitive(value, "blue");
    cJSON *power = cJSON_GetObjectItemCaseSensitive(value, "power");
    led_set_rgb(power->valuedouble * red->valueint, power->valuedouble * green->valueint,
        power->valuedouble * blue->valueint);
    iot_device_value_updated(red->valueint, green->valueint, blue->valueint, power->valuedouble);
  } else if (event_type == Hello) {
    int req_id = cJSON_GetObjectItemCaseSensitive(json, "reqId")->valueint;
    cJSON *res = cJSON_CreateObject();
    cJSON* description = iot_device_get_description();
    cJSON_AddItemToObject(res, "config",  description);

    iot_device_send_response(req_id, res);
  }
  cJSON_Delete(json);
}

void iot_device_init() {
  iot_get_device_uuid(device_uuid);

  color_value = cJSON_CreateObject();
  color_value_red = cJSON_AddNumberToObject(color_value, "red", 0);
  color_value_green = cJSON_AddNumberToObject(color_value, "green", 0);
  color_value_blue = cJSON_AddNumberToObject(color_value, "blue", 0);
  color_value_power = cJSON_AddNumberToObject(color_value, "power", 0);

  led_init();
}

void iot_device_deinit() {
  cJSON_free(color_value);
}

void iot_device_start() {
  iot_init();
}
