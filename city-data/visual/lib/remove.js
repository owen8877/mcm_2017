'use strict'

const fs = require('fs')
const path = require('path')

const scale = 3.14 * 6371000 / 180.0
const step = 300 / scale

let files = process.argv.slice(2)

for (let file of files) {
  let name = path.basename(file, '.json')
  let remove = require(path.resolve(process.env.PWD, `${name}-dump.json`))
  let origin = require(path.resolve(process.env.PWD, `./${name}.json`))

  let newnew = []
  for (let block of origin) {
    let toremove = false
    for (let r of remove) {
      if (r.x == block.x && r.y == block.y)
        toremove = true
    }
    if (!toremove)
      newnew.push(block)
  }

  let outputFilename = `${name}-new.json`
  fs.writeFileSync(outputFilename, JSON.stringify(newnew))
  console.log(`Wrote ${outputFilename}`)
}
