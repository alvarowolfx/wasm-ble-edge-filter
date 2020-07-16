#ifndef DEVICE_CONFIG_H_
#define DEVICE_CONFIG_H_

#include <Arduino.h>
#include <SPIFFS.h>

struct DeviceConfig
{
  // your network SSID (name)
  char wifiSsid[50] = "FullStackIoT";
  // your network password (use for WPA)
  char wifiPass[50] = "FullStackIoT";

  char serverHost[100] = "http://remote-server";
  char serverSendDataPath[50] = "/data";
  char serverCheckScriptPath[50] = "/script";
  char serverDownloadScript[50] = "/script";

  char deviceId[24] = "";
  char deviceName[24] = "";
  int scanTime = 2; //10; // ble scan time in seconds
};

extern DeviceConfig globalConfig;

void setupConfig();

#endif // DEVICE_CONFIG_H_