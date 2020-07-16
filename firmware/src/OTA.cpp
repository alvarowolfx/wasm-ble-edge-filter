#include "OTA.h"
#include "DeviceConfig.h"
#include <Arduino.h>
#include <HTTPClient.h>

void fetchVersionTask(const char *version);

void checkOTATask(void *p)
{
  Serial.println("[OTA] Checking OTA");
  Serial.print("[OTA] Current Rules version: ");
  Serial.println(globalConfig.deviceRuleVersion);

  HTTPClient http;
  http.begin(String(globalConfig.serverHost) + String(globalConfig.serverCheckVersionPath));

  int httpCode = http.GET();
  bool error = false;
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[OTA] GET... code: %d\n", httpCode);

    // url and handler found
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.print("[OTA] Required version: ");
      Serial.println(payload);
      if (strcmp(payload.c_str(), globalConfig.deviceRuleVersion) != 0)
      {
        Serial.println("[OTA] Versions are different, need to download a new version");
        http.end();

        fetchVersionTask(payload.c_str());

        vTaskDelete(NULL);
        return;
      }
      else
      {
        Serial.println("[OTA] Version is good");
      }
    }
  }
  else
  {
    Serial.printf("[OTA] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  vTaskDelete(NULL);
}

void fetchVersionTask(const char *version)
{
  Serial.print("[OTA] Fetching Rules version: ");
  Serial.println(version);

  HTTPClient http;
  snprintf(globalConfig.serverFetchRule, sizeof(globalConfig.serverFetchRule), "/rules/%s/app.wasm", version);
  http.begin(String(globalConfig.serverHost) + String(globalConfig.serverFetchRule));

  int httpCode = http.GET();
  bool error = false;
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[OTA] GET... code: %d\n", httpCode);

    // url and handler found
    if (httpCode == HTTP_CODE_OK)
    {
      Serial.println("[OTA] Fetch WASM OK, writing to file");
      File f = SPIFFS.open("/app.wasm", "w");
      http.writeToStream(&f);
      f.close();
      Serial.println("[OTA] New Wasm version saved");
      saveVersion(version);
    }
  }
  else
  {
    Serial.printf("[OTA] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}