'use strict'

function NugServer() {

  const express = require('express')
  const argv = require('minimist')(process.argv.slice(2))
  const pmongo = require('promised-mongo')
  const morgan = require('morgan')
  const bodyParser = require('body-parser')
  const moment = require('moment')

  const dbConnectionString = 'mongodb://localhost/nug'
  const nugDataPOSTURL = '/nug/api/rawData'
  const nugDataGETURL = '/nug/api/rawData'

  let port, db, rawDataCollection


  let handleNugDataGET = (req, res) => {

    rawDataCollection.find()
      .then(result => {
        res.json(result)
      })
      .catch(err => {
        console.log('caught error on get', err, err.stack)
        res.sendStatus(500)
      })
  }


  let handleNugDataPOST = (req, res) => {
    console.log('post body', JSON.stringify(req.body, null, 2))
    let bodyCopy = JSON.parse(JSON.stringify(req.body))
    bodyCopy.dateTime = moment().toDate()
    rawDataCollection.insert(bodyCopy)
      .then(() => {
        console.log('inserted successfully')
        res.sendStatus(201)
      })
      .catch(err => {
        console.log('error on insert', err, err.stack)
        res.sendStatus(500)
      })

  }


  let init = () => {
    port = argv.port || 4420
    db = pmongo(dbConnectionString)
    rawDataCollection = db.collection('rawData')

    let app = express()

    app.use(morgan('combined'))
    app.use(bodyParser.urlencoded({
      extended: true
    }))

    app.use(bodyParser.json())
    app.get(nugDataGETURL, handleNugDataGET)
    app.post(nugDataPOSTURL, handleNugDataPOST)

    console.log(`listening on ${port}`)
    app.listen(port)
  }

  init()

}

module.exports = new NugServer()
