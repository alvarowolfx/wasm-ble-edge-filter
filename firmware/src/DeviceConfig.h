#ifndef DEVICE_CONFIG_H_
#define DEVICE_CONFIG_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <Preferences.h>
struct DeviceConfig
{
  // your network SSID (name)
  char wifiSsid[50] = "FullStackIoT";
  // your network password (use for WPA)
  char wifiPass[50] = "FullStackIoT";

  char serverHost[100] = "http://192.168.86.118:3000";
  char serverSendDataPath[50] = "/data";
  char serverCheckVersionPath[50] = "/devices/%s/version";
  char serverFetchRule[50] = "/rules/%s/app.wasm";

  char deviceId[24] = "";
  char deviceName[24] = "";
  int scanTime = 2; //10; // ble scan time in seconds

  char deviceRuleVersion[24] = "";
};

extern DeviceConfig globalConfig;

void setupConfig();
void saveVersion(const char *version);

#endif // DEVICE_CONFIG_H_