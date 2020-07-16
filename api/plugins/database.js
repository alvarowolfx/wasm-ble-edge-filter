'use strict'

const fp = require('fastify-plugin')
const Datastore = require('nedb')

const historicalDb = new Datastore({
  filename: '../historicalStorage.db',
  autoload: true,
})
const devicesDb = new Datastore({
  filename: '../devicesStorage.db',
  autoload: true,
})
const rulesDb = new Datastore({
  filename: '../rulesStorage.db',
  autoload: true,
})

module.exports = fp(async function (fastify, opts) {
  fastify.decorate('historicalDb', historicalDb)
  fastify.decorate('devicesDb', devicesDb)
  fastify.decorate('rulesDb', rulesDb)
})
