/*
 * settings.h
 *
 *  Created on: Jan 5, 2021
 *      Author: pwiklowski
 */


char* audience = "https%3A%2F%2Fwiklosoft.eu.auth0.com%2Fapi%2Fv2%2F";
char* scope = "name+email+profile+openid+offline_access";

char* AUTH_TOKEN_URL =  "https://wiklosoft.eu.auth0.com/oauth/token";
char* AUTH_CODE_URL = "https://wiklosoft.eu.auth0.com/oauth/device/code";

char* OTA_IMAGE_URL = "https://ota.wiklosoft.com/iot-rgb-esp32.bin";

//char* IOT_SERVER_URL_TEMPLATE = "ws://192.168.1.28:8000/device?token=%s";
char*  IOT_SERVER_URL_TEMPLATE ="wss://iot.wiklosoft.com/connect/device?token=%s";

int SOFTWARE_UPDATE_CHECK_INTERVAL_MIN = 24*60;

int TOKEN_REFRESH_INTERVAL_MIN = 12*60;

char* WIFI_SSID = "";
char* WIFI_PASS = "";

char* CLIENT_ID = "";
char* CLIENT_SECRET =  "";
