'use strict'

module.exports = async function (fastify, opts) {
  fastify.get('/:deviceId', function (request, reply) {
    const { deviceId } = request.params
    this.devicesDb.findOne({ _id: deviceId }, (err, docs) => {
      reply.send(docs)
    })
  })

  fastify.post('/:deviceId', function (request, reply) {
    const { deviceId } = request.params
    this.devicesDb.insert({ _id: deviceId, createdAt: Date.now() }, (err, device) => {
      reply.send(device)
    })
  })

  fastify.get('/:deviceId/history', function (request, reply) {
    const { deviceId } = request.params
    this.historicalDb.find({ deviceId }).sort({ time: -1 }).exec( (err, docs) => {
      reply.send(docs)
    })
  })

  fastify.post('/:deviceId/history', function (request, reply) {
    const { deviceId } = request.params
    const insertedData = request.body.map( data => ({
      deviceId,
      time: Date.now(),
      ...data
    }))
    this.historicalDb.insert(insertedData, (err, data) => {
      reply.send(data)
    })
  })

  fastify.get('/:deviceId/version', function (request, reply) {
    const { deviceId } = request.params
    this.devicesDb.findOne({ _id: deviceId }, (err, device) => {
      reply.send((device && device.versionName) || "")
    })
  })

  fastify.put('/:deviceId/rules/:versionName', function (request, reply) {
    const { deviceId, versionName } = request.params
    this.devicesDb.update({ _id: deviceId}, { '$set' : { versionName } }, { upsert: true, returnUpdatedDocs: true }, (err) => {
      reply.send({ "message" : "updated" })
    })
  })

  fastify.get('/', function (request, reply) {
    this.devicesDb.find({}, (err, docs) => {
      reply.send(docs)
    })
  })
}
