#ifndef WASM_RULES_ENGINE_H_
#define WASM_RULES_ENGINE_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <m3_core.h>
#include <m3_api_defs.h>
#include <m3_env.h>

#define WASM_STACK_SLOTS 8 * 1024
#define NATIVE_STACK_SIZE 32 * 1024

void setupRulesEngine();
bool filterDevice(std::string name, std::string address, std::string serviceData, std::string serviceUuid, int rssi);

#endif // WASM_RULES_ENGINE_H_