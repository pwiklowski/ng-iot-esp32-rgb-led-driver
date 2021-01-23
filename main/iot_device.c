#include "cJSON.h"
#include "iot.h"
#include "led.h"
#include <stdio.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "led.h"
#include <string.h>
#include "iot_device.h"

char *TAG = "IOT_DEVICE";

#define SCHEMA_RGB "{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"type\":\"object\",\"properties\":{\"red\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255}, \"green\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255}, \"blue\":{\"type\":\"integer\",\"minimum\":0,\"maximum\":255},\"power\":{\"type\":\"number\",\"minimum\":0,\"maximum\":1}},\"required\":[\"red\", \"green\", \"blue\",\"power\"],\"additionalProperties\":false}"
#define SCHEMA_SINGLE_CHANNEL "{\"$schema\":\"http://json-schema.org/draft-04/schema#\",\"type\":\"object\",\"properties\":{\"power\":{\"type\":\"number\",\"minimum\":0,\"maximum\":1}},\"required\":[\"power\"],\"additionalProperties\":false}"

char device_uuid[38];

#if defined(VARIANT_3CH_RGB) || defined(VARIANT_6CH_RGB)
char *VARIABLE_UUID = "56be315a-05b2-4f5e-8fd1-342b40c006fe";

cJSON *color_value;
cJSON *color_value_red;
cJSON *color_value_green;
cJSON *color_value_blue;
cJSON *color_value_power;

#endif

#if defined(VARIANT_6CH)
cJSON *value_ch4;
cJSON *value_ch4_power;
char *VARIABLE_CH4_UUID = "d2e15ddd-95bb-4885-b63b-00f810dbfaa4";

cJSON *value_ch5;
cJSON *value_ch5_power;
char *VARIABLE_CH5_UUID = "d2e15ddd-95bb-4885-b63b-00f810dbfaa5";

cJSON *value_ch6;
cJSON *value_ch6_power;
char *VARIABLE_CH6_UUID = "d2e15ddd-95bb-4885-b63b-00f810dbfaa6";

extern ledc_channel_config_t led_channel_4;
extern ledc_channel_config_t led_channel_5;
extern ledc_channel_config_t led_channel_6;
#endif



#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB) || defined(VARIANT_3CH)

cJSON *value_ch1;
cJSON *value_ch1_power;
char *VARIABLE_CH1_UUID = "d2e15ddd-95bb-4885-b63b-00f810dbfaa1";

cJSON *value_ch2;
cJSON *value_ch2_power;
char *VARIABLE_CH2_UUID = "d2e15ddd-95bb-4885-b63b-00f810dbfaa2";

cJSON *value_ch3;
cJSON *value_ch3_power;
char *VARIABLE_CH3_UUID = "d2e15ddd-95bb-4885-b63b-00f810dbfaa3";

extern ledc_channel_config_t led_channel_1;
extern ledc_channel_config_t led_channel_2;
extern ledc_channel_config_t led_channel_3;

#endif

cJSON* iot_device_get_description() {
  char version[32];
  char name[32];
  iot_get_app_version(name, version);

  cJSON *device = cJSON_CreateObject();
  cJSON_AddStringToObject(device, "name", name);
  cJSON_AddStringToObject(device, "deviceUuid", device_uuid);
  cJSON_AddStringToObject(device, "version", version);
  cJSON *vars = cJSON_AddObjectToObject(device, "vars");

#if defined(VARIANT_3CH_RGB) || defined(VARIANT_6CH_RGB)
  iot_create_variable_description(vars, VARIABLE_UUID, "color", "rw", SCHEMA_RGB, cJSON_Duplicate(color_value, true));
#endif

#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB) || defined(VARIANT_3CH)
  iot_create_variable_description(vars, VARIABLE_CH1_UUID, "ch1", "rw", SCHEMA_SINGLE_CHANNEL, cJSON_Duplicate(value_ch1, true));
  iot_create_variable_description(vars, VARIABLE_CH2_UUID, "ch2", "rw", SCHEMA_SINGLE_CHANNEL, cJSON_Duplicate(value_ch2, true));
  iot_create_variable_description(vars, VARIABLE_CH3_UUID, "ch3", "rw", SCHEMA_SINGLE_CHANNEL, cJSON_Duplicate(value_ch3, true));
#endif

#if defined(VARIANT_6CH)
  iot_create_variable_description(vars, VARIABLE_CH4_UUID, "ch4", "rw", SCHEMA_SINGLE_CHANNEL, cJSON_Duplicate(value_ch4, true));
  iot_create_variable_description(vars, VARIABLE_CH5_UUID, "ch5", "rw", SCHEMA_SINGLE_CHANNEL, cJSON_Duplicate(value_ch5, true));
  iot_create_variable_description(vars, VARIABLE_CH6_UUID, "ch6", "rw", SCHEMA_SINGLE_CHANNEL, cJSON_Duplicate(value_ch6, true));
#endif


  return device;
}

void iot_set_value(char* variable_uuid, cJSON*newValue, cJSON* value, ledc_channel_config_t channel) {
  cJSON *newPower = cJSON_GetObjectItemCaseSensitive(newValue, "power");
  cJSON *power = cJSON_GetObjectItemCaseSensitive(value, "power");
  led_set_value(channel, LED_MAX_VALUE * newPower->valuedouble);
  cJSON_SetNumberValue(power, newPower->valuedouble);
  iot_send_value_changed_notifcation(device_uuid, variable_uuid, value);
  ESP_LOGI(TAG, "iot_device_value_updated %s %d",variable_uuid, (int)(LED_MAX_VALUE * newPower->valuedouble));
}

void iot_device_event_handler(const char *payload, const size_t len) {
  cJSON *json = cJSON_Parse((char*) payload);
  int event_type = cJSON_GetObjectItemCaseSensitive(json, "type")->valueint;

  if (event_type == SetValue) {
    cJSON *args = cJSON_GetObjectItemCaseSensitive(json, "args");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(args, "value");
    cJSON *variableUuid = cJSON_GetObjectItemCaseSensitive(args, "variableUuid");
    ESP_LOGI(TAG, "iot_device_value_updated %s",variableUuid->valuestring);

#if defined(VARIANT_3CH_RGB) || defined(VARIANT_6CH_RGB)
    if (strcmp(variableUuid->valuestring, VARIABLE_UUID) == 0) {
      cJSON *red = cJSON_GetObjectItemCaseSensitive(value, "red");
      cJSON *green = cJSON_GetObjectItemCaseSensitive(value, "green");
      cJSON *blue = cJSON_GetObjectItemCaseSensitive(value, "blue");
      cJSON *power = cJSON_GetObjectItemCaseSensitive(value, "power");
      led_set_rgb(power->valuedouble * red->valueint,
          power->valuedouble * green->valueint,
          power->valuedouble * blue->valueint);

      cJSON_SetNumberValue(color_value_power, power->valuedouble);
      cJSON_SetNumberValue(color_value_red, red->valueint);
      cJSON_SetNumberValue(color_value_green, green->valueint);
      cJSON_SetNumberValue(color_value_blue, blue->valueint);

      iot_send_value_changed_notifcation(device_uuid, VARIABLE_UUID, color_value);
    }
#endif

#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB) || defined(VARIANT_3CH)
    if( strcmp(variableUuid->valuestring, VARIABLE_CH1_UUID) == 0) {
      iot_set_value(VARIABLE_CH1_UUID, value, value_ch1, led_channel_1);
    } else if( strcmp(variableUuid->valuestring, VARIABLE_CH2_UUID) == 0) {
      iot_set_value(VARIABLE_CH2_UUID, value, value_ch2, led_channel_2);
    } else if( strcmp(variableUuid->valuestring, VARIABLE_CH3_UUID) == 0) {
      iot_set_value(VARIABLE_CH3_UUID, value, value_ch3, led_channel_3);
    }
#endif

#if defined(VARIANT_6CH)
    if( strcmp(variableUuid->valuestring, VARIABLE_CH4_UUID) == 0) {
      iot_set_value(VARIABLE_CH4_UUID, value, value_ch4, led_channel_4);
    } else if( strcmp(variableUuid->valuestring, VARIABLE_CH5_UUID) == 0) {
      iot_set_value(VARIABLE_CH5_UUID, value, value_ch5, led_channel_5);
    } else if( strcmp(variableUuid->valuestring, VARIABLE_CH6_UUID) == 0) {
      iot_set_value(VARIABLE_CH6_UUID, value, value_ch6, led_channel_6);
    }
#endif

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

#if defined(VARIANT_3CH_RGB) || defined(VARIANT_6CH_RGB)
  color_value = cJSON_CreateObject();
  color_value_red = cJSON_AddNumberToObject(color_value, "red", 0);
  color_value_green = cJSON_AddNumberToObject(color_value, "green", 0);
  color_value_blue = cJSON_AddNumberToObject(color_value, "blue", 0);
  color_value_power = cJSON_AddNumberToObject(color_value, "power", 0);
#endif

#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB) || defined(VARIANT_3CH)
  value_ch1 = cJSON_CreateObject();
  value_ch1_power = cJSON_AddNumberToObject(value_ch1, "power", 0);

  value_ch2 = cJSON_CreateObject();
  value_ch2_power = cJSON_AddNumberToObject(value_ch2, "power", 0);

  value_ch3 = cJSON_CreateObject();
  value_ch3_power = cJSON_AddNumberToObject(value_ch3, "power", 0);
#endif

#ifdef VARIANT_6CH
  value_ch4 = cJSON_CreateObject();
  value_ch4_power = cJSON_AddNumberToObject(value_ch4, "power", 0);

  value_ch5 = cJSON_CreateObject();
  value_ch5_power = cJSON_AddNumberToObject(value_ch5, "power", 0);

  value_ch6 = cJSON_CreateObject();
  value_ch6_power = cJSON_AddNumberToObject(value_ch6, "power", 0);
#endif

  led_init();
}

void iot_device_deinit() {
#if defined(VARIANT_3CH_RGB) || defined(VARIANT_6CH_RGB)
  cJSON_free(color_value);
#endif

#if defined(VARIANT_6CH) || defined(VARIANT_6CH_RGB) || defined(VARIANT_3CH)
  cJSON_free(value_ch1);
  cJSON_free(value_ch2);
  cJSON_free(value_ch3);
#endif

#ifdef VARIANT_6CH
  cJSON_free(value_ch4);
  cJSON_free(value_ch5);
  cJSON_free(value_ch6);
#endif
}

void iot_device_start() {
  iot_init();
}
