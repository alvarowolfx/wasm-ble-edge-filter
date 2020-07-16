# Using WASM as a Rules Engine for IoT

The idea here is to have an IoT device collecting data and the data filtering happens at the edge and is defined by a Wasm code running on the device.

For this demo, I used an ESP32 scanning for BLE devices and the firmware passes the BLE device info like device name, serviceUUID, RSSID and other things, so the Wasm code can return true/false if that device should be filtered or not.

_This is a work in progress_

## Project Components

### Device Firmware

`firmware` folder

Code wrote using Arduino Framework, Nimble BLE Stack and Wasm3. All wrote using PlatformIO.

Main tasks

- Load Wasm code from SPIFFS (under `/app.wasm` ) and pre cache the filter function.
  - Code can the uploaded via USB using
  - The device also exposes some APIs to be called from the Wasm land to get the BLE device info
    - See [WasmRulesEngine.cpp](firmware/src/WasmRulesEngine.cpp#74)
    - See [rules.ts](wasm_apps/assemblyscript/rules.ts)
- Checks after connecting to WiFi which version of the rule if should be running and download it if needed
  - Uses the Rules Engine API for that.
- Scan BLE devices and filter using the Wasm code, if is available.
- [Work In Progress] - Post data to Rules Engine API

### Rules Engine API

`api` folder

The backend was built using Node.js and `fastify`. It's just a proof of concepts, but is basically have device management endpoints and more important for this demo, some endpoints to send Assemblyscript code, compile it and save as a rule to be used/downloaded later by the device.

The backend exposes those endpoints bellow.

Device Management

- GET /devices - List devices
- POST /:deviceId - Create device with given ID,
- GET /:deviceId/history - Fetch device data history,
- POST /:deviceId/history - Save device data
- GET /:deviceId/version - Used by the device to determine which version of the rule it should be runnig
- PUT /:deviceId/rules/:versionName - Update which version the device should be running

Rules Management

- GET /rules - List created rules
- POST /:versionName - Create rule with the given versionName. The body must contain Assemblyscript code. See an example on the next section.
- GET /:versionName/app.wasm - Return the raw Wasm file to be downloaded by the device
- GET /:versionName - Show info of the rule

### Wasm Example Script

`wasm_apps` folder

Example of a business logic that will be compiled to run on the device. For now the folder contains an example using Assemblyscript.

A more advance user will write this kind of code to run on the edge device.

```
function filter(device: Device): bool {
  if (device.name == "OBDII") {
    // I want to track my car
    return false;
  }

  if (device.serviceUUID == "0xfeaa") {
    // Always send Eddystone tags
    return false;
  }

  return device.rssi <= -75; // Too Far
}
```

### Update WASM file via USB

The file is being saved on SPIFFS and the Arduino is loading from it to run on WASM3 VM. So we can update the WASM code without changing the host code.

Steps to update WASM code:

- `cd` into `wasm_apps/assemblyscript` folder and run `npm run build`.
  - This will compile the AS code to Wasm and also copy the file to the `data` folder.
  - Run the command `pio run --target uploadfs` on the `firmware` folder.

#### References

- https://github.com/AssemblyScript/assemblyscript
- https://github.com/wasm3/wasm3
