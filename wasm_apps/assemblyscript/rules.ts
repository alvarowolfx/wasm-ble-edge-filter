@external("engine", "getDeviceName")
declare function _getDeviceName(ptr: usize): i32;

@external("engine", "getDeviceAddress")
declare function _getDeviceAddress(ptr: usize): i32;

@external("engine", "getDeviceServiceData")
declare function _getDeviceServiceData(ptr: usize): i32;

@external("engine", "getDeviceServiceUUID")
declare function _getDeviceServiceUuid(ptr: usize): i32;

@external("engine", "getDeviceRssi")
declare function _getDeviceRssi(): i32;

export class Device {
  name: string = "";
  address: string = "";
  serviceData: string = "";
  serviceUUID: string = "";
  rssi: i32 = 0;
}

function getDeviceName(): string {
  const ptr = memory.data(24);
  const len = _getDeviceName(ptr);
  return String.UTF8.decodeUnsafe(ptr, len, true);
}

function getDeviceAddress(): string {
  const ptr = memory.data(24);
  const len = _getDeviceAddress(ptr);
  return String.UTF8.decodeUnsafe(ptr, len, true);
}

function getDeviceServiceData(): string {
  const ptr = memory.data(24);
  const len = _getDeviceServiceData(ptr);
  return String.UTF8.decodeUnsafe(ptr, len, true);
}

function getDeviceServiceUuid(): string {
  const ptr = memory.data(24);
  const len = _getDeviceServiceUuid(ptr);
  return String.UTF8.decodeUnsafe(ptr, len, true);
}

export function getDevice(): Device {
  const device = new Device();
  device.name = getDeviceName();
  device.address = getDeviceAddress();
  device.serviceData = getDeviceServiceData();
  device.serviceUUID = getDeviceServiceUuid();
  device.rssi = _getDeviceRssi();
  return device;
}
