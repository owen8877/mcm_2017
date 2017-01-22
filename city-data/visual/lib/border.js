'use strict'

const request = require('request-promise')
const cheerio = require('cheerio')
const fs = require('fs')

let place = process.argv[2]

let startLat, startLng, urln
if (place == "nottingham") {
  startLat = 53.019033
  startLng = -1.258524
  urln = '123292'
} else if (place == "barrie") {
  startLat = 44.425934
  startLng = -79.747266
  urln = '6609999'
} else {
  exit(1)
}
const scale = 3.14 * 6371000 / 180.0
const step = 300 / scale

request.get(`https://www.openstreetmap.org/api/0.6/relation/${urln}`)
  .then(body => {
    let $ = cheerio.load(body)

    // iterate ways
    let root = $('osm').find('relation')[0]

    let routes = []

    while (true) {
      let node = root.children[0].next
      if (!node)
        break

      if (node.attribs.type == "way") {
        //console.log(node.attribs)
        routes.push(node.attribs.ref)
      }
      root = node
    }
    return routes
  })
  .then(routes => {
    return Promise.all(routes.map(route => {
      return request.get(`https://www.openstreetmap.org/api/0.6/way/${route}`)
        .then(body => {
          let $ = cheerio.load(body)
          // iterate ways
          let root = $('osm').find('way')[0]

          let nodes = []

          while (true) {
            let node = root.children[0].next
            if (!node)
              break

            if (node.attribs.ref) {
              //console.log(node.attribs)
              nodes.push(node.attribs.ref)
            }
            root = node
          }
          return nodes
        })
    }))
  })
  .then(nodess => {
    let nodes = []
    for (let node_s of nodess) {
      nodes = nodes.concat(node_s)
    }
    return nodes
  })
  .then(nodes => {
    return Promise.all(nodes.map(node => {
      return request.get(`https://www.openstreetmap.org/api/0.6/node/${node}`)
        .then(body => {
          let $ = cheerio.load(body)
          // iterate ways
          let root = $('osm').find('node')[0]
          return {lat: root.attribs.lat, lng: root.attribs.lon}
        })
    }))
  })
  .then(wtf => {
    return wtf.map(coord => {
      let lat = coord.lat, lng = coord.lng
      let y = (startLat - lat) / step, x = (lng - startLng) / step
      x = parseInt(parseFloat(x) * 5) / 5
      y = parseInt(parseFloat(y) * 5) / 5
      return {x, y}
    })
  })
  .then(wtf => {
    fs.writeFile(`border/${place}-border.json`, JSON.stringify(wtf), e => {
      if (e)
        throw e
      console.log('Success')
    })
  })

