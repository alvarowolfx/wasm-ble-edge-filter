#include "DeviceConfig.h"

void setupConfig()
{
  uint64_t chipId = ESP.getEfuseMac();
  uint32_t highBytesChipId = (uint32_t)(chipId >> 16); // High 4 bytes
  //uint16_t lowBytesChipId = (uint16_t)chipId; // Low 2 bytes
  snprintf(globalConfig.deviceId, sizeof(globalConfig.deviceId), "%08X", highBytesChipId);
  snprintf(globalConfig.deviceName, sizeof(globalConfig.deviceName), "GATEWAY_%08X", highBytesChipId);
}
