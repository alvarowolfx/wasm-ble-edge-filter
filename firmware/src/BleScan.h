#ifndef BLE_SCAN_H_
#define BLE_SCAN_H_

#include <FS.h>
#include <SPIFFS.h>
#include "DeviceConfig.h"

extern DeviceConfig globalConfig;

void scannerTask(void *);
void setupScanner();

#endif // BLE_SCAN_H_