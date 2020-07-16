'use strict'

const asc = require('assemblyscript/cli/asc')

const baseWasmModule = `
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

export function _filter(): bool {
  const device = getDevice();
  return filter(device);
}
`

module.exports = async function (fastify, opts) {

  fastify.get('/', function (request, reply) {
    this.rulesDb.find({}, (err, versions) => {
      reply.send(versions.map( v => ({
        versionName : v._id,
        byteLength : v.byteLength,
        createdAt: v.createdAt
      })))
    })
  })

  fastify.post('/:versionName', async function (request, reply) {
    const { versionName } = request.params
    const code = request.body
    return new Promise( (resolve, reject) => {
      const finalCode = `${baseWasmModule}\n${code}`
      const { binary, text, stdout, stderr } = asc.compileString(finalCode, {
        runtime : 'half',
        noAssert : true,
        optimizeLevel : 3,
        shrinkLevel: 2,
        converge: true,
        optimize: true,
        use : 'abort='
      })
      const rule = {
        _id: versionName,
        raw : Buffer.from(binary).toString('hex'),
        byteLength : binary.byteLength,
        code : finalCode,
        wat : text,
        createdAt: Date.now()
      }
      this.rulesDb.insert(rule, (err, version) => {
        reply.send(version)
        resolve()
      })
    })
  })

  fastify.get('/:versionName/app.wasm', function (request, reply) {
    const { versionName } = request.params
    this.rulesDb.findOne({ _id: versionName }, (err, version) => {
      const buf = Buffer.from(version.raw, 'hex')
      reply.send(buf)
    })
  })

  fastify.get('/:versionName', function (request, reply) {
    const { versionName } = request.params
    this.rulesDb.findOne({ _id: versionName }, (err, version) => {
      reply.send(version)
    })
  })

}
