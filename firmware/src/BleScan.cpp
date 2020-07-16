#include "BleScan.h"
#include "DeviceConfig.h"
#include "WasmRulesEngine.h"

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

#include <sstream>

BLEScan *pBLEScan;

void setupScanner()
{
  BLEDevice::init("");
}

BLEScanResults scanBleDevices()
{
  pBLEScan = BLEDevice::getScan(); //create new scan

  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  BLEScanResults foundDevices = pBLEScan->start(globalConfig.scanTime, false);
  return foundDevices;
}

void scannerTask(void *p)
{
  setupRulesEngine();

  while (1)
  {
    Serial.println("[SCANNER] Scanning...");
    BLEScanResults devices = scanBleDevices();
    Serial.print("[SCANNER] Found ");
    Serial.print(devices.getCount());
    Serial.println(" devices...");

    int count = devices.getCount();
    std::stringstream out;
    out << "[";
    int selectedDevices = 0;
    for (int i = 0; i < count; i++)
    {
      BLEAdvertisedDevice device = devices.getDevice(i);
      std::string address = device.getAddress().toString();
      int rssi = device.getRSSI();
      std::string name = "";
      std::string serviceData = "";
      std::string serviceUuid = "";
      if (device.haveName())
      {
        name = device.getName();
      }

      if (device.haveServiceData())
      {
        serviceData = device.haveServiceData();
      }

      if (device.haveServiceUUID())
      {
        serviceUuid = device.getServiceUUID().toString();
      }

      bool filtered = filterDevice(name, address, serviceData, serviceUuid, rssi);
      Serial.print("[SCANNER] Found - ");
      Serial.print(address.c_str());
      Serial.print(" - ");
      Serial.print(name.c_str());
      Serial.print(" - ");
      Serial.print(serviceData.c_str());
      Serial.print(" - ");
      Serial.print(serviceUuid.c_str());
      Serial.print(" - rssi (");
      Serial.print(rssi);
      Serial.print(") ");
      Serial.println(filtered ? "Filtered" : "Not Filtered");

      if (!filtered)
      {
        if (selectedDevices > 0)
        {
          out << ",";
        }
        selectedDevices++;
        out << "{";
        out << "\"address\": \"" << address << "\",";
        out << "\"name\": \"" << name << "\",";
        out << "\"serviceData\": \"" << serviceData << "\",";
        out << "\"serviceUUID\": \"" << serviceUuid << "\",";
        out << "\"rssi\": " << rssi << "";
        out << "}";
      }
    }
    out << "]";
    Serial.println(out.str().c_str());
    pBLEScan->clearResults();

    vTaskDelay(30 * 1000 / portTICK_PERIOD_MS);
  }
}