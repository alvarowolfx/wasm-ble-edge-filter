
#include "WasmRulesEngine.h"
#include <Arduino.h>

static IM3Runtime wasmRuntime;
static IM3Module module;
static IM3Environment env;
static IM3Function filterFunction;
int currentDeviceRssi;

m3ApiRawFunction(m3_get_device_rssi)
{
  m3ApiReturnType(int);
  //Serial.println("[Engine] Calling bridge - getDeviceRssi");

  m3ApiReturn(currentDeviceRssi);
}

#define m3ApiReturnStringFunction(func, stringVar) \
  std::string stringVar;                           \
  m3ApiRawFunction(func)                           \
  {                                                \
    m3ApiReturnType(uint32_t);                     \
    m3ApiGetArgMem(char *, out);                   \
    const char *name = stringVar.c_str();          \
    uint32_t len = strlen(name);                   \
    memcpy(out, name, len);                        \
    out[len] = '\0';                               \
    m3ApiReturn(len);                              \
  }

m3ApiReturnStringFunction(m3_get_device_name, currentDeviceName);
m3ApiReturnStringFunction(m3_get_device_address, currentDeviceAddress);
m3ApiReturnStringFunction(m3_get_device_service_data, currentDeviceServiceData);
m3ApiReturnStringFunction(m3_get_device_service_uuid, currentDeviceServiceUUID);

void clearDevice()
{
  currentDeviceName = "";
  currentDeviceAddress = "";
  currentDeviceServiceData = "";
  currentDeviceServiceUUID = "";
  currentDeviceRssi = 0;
}

m3ApiRawFunction(m3_arduino_print)
{
  // Serial.println("[Engine] Calling bridge - print");
  m3ApiGetArgMem(const byte *, out);
  m3ApiGetArg(int32_t, out_len);

  byte buff[out_len + 1];
  memcpy(buff, out, out_len);
  buff[out_len] = '\0';

  Serial.print((char *)buff);

  m3ApiSuccess();
}

// Dummy, for TinyGO
m3ApiRawFunction(m3_dummy)
{
  m3ApiSuccess();
}

M3Result linkLocalFunctions(IM3Runtime runtime)
{
  IM3Module module = runtime->modules;
  const char *engine = "engine";
  const char *serial = "serial";

  /* Engine */
  m3_LinkRawFunction(module, engine, "getDeviceRssi", "i()", &m3_get_device_rssi);
  m3_LinkRawFunction(module, engine, "getDeviceName", "i(*)", &m3_get_device_name);
  m3_LinkRawFunction(module, engine, "getDeviceAddress", "i(*)", &m3_get_device_address);
  m3_LinkRawFunction(module, engine, "getDeviceServiceData", "i(*)", &m3_get_device_service_data);
  m3_LinkRawFunction(module, engine, "getDeviceServiceUUID", "i(*)", &m3_get_device_service_uuid);

  /* Serial */
  m3_LinkRawFunction(module, serial, "print", "v(*i)", &m3_arduino_print);

  // Dummy (for TinyGo)
  m3_LinkRawFunction(module, "env", "io_get_stdout", "i()", &m3_dummy);

  return m3Err_none;
}

size_t readWasmFileSize(const char *path)
{
  Serial.printf("[Engine] Reading file size: %s\n", path);

  if (!SPIFFS.exists(path))
  {
    Serial.println("[Engine] File not found");
    return 0;
  }

  File file = SPIFFS.open(path);
  if (!file)
  {
    Serial.println("[Engine] Failed to open file for reading");
    return 0;
  }
  size_t size = file.size();
  file.close();
  Serial.printf("[Engine] Wasm File size: %d\n", size);
  return size;
}

size_t readWasmFile(const char *path, uint8_t *buf)
{
  Serial.printf("[Engine] Reading file: %s\n", path);

  if (!SPIFFS.exists(path))
  {
    Serial.println("[Engine] File not found");
    return 0;
  }

  File file = SPIFFS.open(path);
  if (!file)
  {
    Serial.println("[Engine] Failed to open file for reading");
    return 0;
  }

  Serial.println("[Engine] Read from file: ");
  size_t i = 0;
  while (file.available())
  {
    buf[i] = file.read();
    i++;
  }
  file.close();

  return i;
}

#define FATAL_MSG(func, msg)                   \
  {                                            \
    Serial.print("[Engine] Fatal: " func " "); \
    Serial.println(msg);                       \
  }

#define FATAL(func, msg) \
  {                      \
    FATAL_MSG(func, msg) \
    return;              \
  }

void setupRulesEngine()
{
  SPIFFS.begin(true);

  M3Result result = m3Err_none;

  size_t app_wasm_size = readWasmFileSize("/app.wasm");
  if (app_wasm_size == 0)
    FATAL("ReadWasm", "File not found")

  uint8_t buffer[app_wasm_size];
  size_t read_bytes = readWasmFile("/app.wasm", buffer);
  if (read_bytes == 0)
    FATAL("ReadWasm", "File not found")

  Serial.println("[Engine] Wasm file read");

  env = m3_NewEnvironment();
  if (!env)
  {
    FATAL_MSG("NewEnvironment", "failed");
    return;
  }

  wasmRuntime = m3_NewRuntime(env, WASM_STACK_SLOTS, NULL);
  if (!wasmRuntime)
  {
    FATAL_MSG("NewRuntime", "failed");
    return;
  }

  Serial.println("[Engine] Wasm Runtime created");

  result = m3_ParseModule(env, &module, buffer, app_wasm_size - 1);
  if (result)
    FATAL("ParseModule", result);

  Serial.println("[Engine] Wasm Module parsed");

  result = m3_LoadModule(wasmRuntime, module);
  if (result)
    FATAL("LoadModule", result);

  Serial.println("[Engine] Wasm Module loaded");

  result = linkLocalFunctions(wasmRuntime);
  if (result)
    FATAL("LinkArduino", result);

  Serial.println("[Engine] Wasm Link Functions done");

  result = m3_FindFunction(&filterFunction, wasmRuntime, "_filter");
  if (result)
  {
    Serial.println("[Engine] Failed to find filter function : ");
    Serial.println(result);
    return;
  }

  Serial.println("[Engine] Found filter function");

  Serial.println("[Engine] Warmup with function");
  result = m3_Call(filterFunction);

  Serial.println("[Engine] Finished warmup");
}

bool filterDevice(std::string name, std::string address, std::string serviceData, std::string serviceUuid, int rssi)
{
  M3Result result = m3Err_none;
  //printf("[Engine] Running WebAssembly...\n");

  if (filterFunction != NULL)
  {
    currentDeviceName = name;
    currentDeviceAddress = address;
    currentDeviceServiceData = serviceData;
    currentDeviceServiceUUID = serviceUuid;
    currentDeviceRssi = rssi;

    result = m3_Call(filterFunction);

    if (result)
    {
      M3ErrorInfo info;
      m3_GetErrorInfo(wasmRuntime, &info);
      Serial.print("Error: ");
      Serial.print(result);
      Serial.print(" (");
      Serial.print(info.message);
      Serial.println(")");
      return false;
    }

    long value = *(uint64_t *)(wasmRuntime->stack);
    clearDevice();
    return value == 1l;
  }
  clearDevice();
  return false;
}
