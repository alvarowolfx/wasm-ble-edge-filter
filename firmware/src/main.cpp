
#include <Arduino.h>

#include "Network.h"
#include "BleScan.h"
#include "WasmRulesEngine.h"
#include "DeviceConfig.h"
#include "DeviceState.h"

DeviceConfig globalConfig;
DeviceState globalState;

void setup()
{
  // Setup reset pin
  Serial.begin(115200);
  SPIFFS.begin(true);

  setupConfig();

  Serial.print("[INIT] Device ID: ");
  Serial.println(globalConfig.deviceName);

  Serial.print("[INIT] Rules version: ");
  Serial.println(globalConfig.deviceRuleVersion);

  setupScanner();

  xTaskCreatePinnedToCore(&networkTask, "network", 4 * 1024, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(&scannerTask, "scanner", NATIVE_STACK_SIZE, NULL, 4, NULL, 0);
}

void loop()
{
  delay(100);
}
