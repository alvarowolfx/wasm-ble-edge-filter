import { Device, getDevice } from "./rules";
//import * as Serial from "./serial";

export function _filter(): bool {
  const device = getDevice();
  return filter(device);
}

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
