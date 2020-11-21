#ifndef __IOT_CONNECTION_H__
#define __IOT_CONNECTION_H__

enum MessageType {
  Hello,
  GetValue,
  GetDevices,
  Notification,
  DeviceConnected,
  DeviceDisconnected,
  ValueUpdated,
  SetValue,
  GetDevice
};


void websocket_app_start();

#endif // __IOT_CONNECTION_H__