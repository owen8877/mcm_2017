'use strict'

const fs = require('fs')
const path = require('path')

const scale = 3.14 * 6371000 / 180.0
const step = 300 / scale

let files = process.argv.slice(2)

for (let file of files) {
  let patch = require(path.resolve(process.env.PWD, file))
  let name = path.basename(file, '.js')
  let origin = require(path.resolve(process.env.PWD, `./${name}.json`))

  let diff = []

  let startLat = patch.coord[0], startLng = patch.coord[1]
  for (let block of origin) {
    for (let p of patch.patch) {
      // first check bus
      if (p.bus) {
        let _x, _y
        _x = Math.floor((startLat - p.range.coord[0]) / step)
        _y = Math.floor((p.range.coord[1] - startLng) / step)
        if (block.x == _x && block.y == _y) {
          console.log(`Hit ${p.name} at x-${block.x} y-${block.y}`)
          block.bus = true
          diff.push({
            x: block.x,
            y: block.y,
            type: block.type,
            progress: 0,
            bus: true
          })
        }
        continue
      }

      let typeCheck
      if (block.type == 'u') {
        typeCheck = true
      } else if (p.type == null) {
        typeCheck = true
      } else {
        typeCheck = p.type == block.type
      }
      if (!typeCheck)
        continue

      let rangeCheck
      let range = p.range
      if (range.circle) {
        let dis
        let _x2 = Math.pow((block.x - (startLat - range.coord[0]) / step), 2)
        let _y2 = Math.pow((block.y - (range.coord[1] - startLng) / step), 2)
        dis = Math.sqrt(_x2 + _y2)
        if (dis < range.r / 300)
          rangeCheck = true
      } else if (range.rect) {
        let lefttop, rightbottom
        lefttop = [
          (startLat - range.coord[0][0]) / step,
          (range.coord[0][1] - startLng) / step
        ]
        rightbottom = [
          (startLat - range.coord[1][0]) / step,
          (range.coord[1][1] - startLng) / step
        ]
        rangeCheck = block.x > lefttop[0] && block.x < rightbottom[0] && block.y > lefttop[1] && block.y < rightbottom[1]
      }
      if (!rangeCheck)
        continue

      // okay, add point
      console.log(`Hit ${p.name} at x-${block.x} y-${block.y}`)
      if (block.type != 'u'){
        block.progress++
      } else {
        block.progress = 1
        block.type = p.type || 'w'
      }
      diff.push({
        x: block.x,
        y: block.y,
        type: block.type,
        progress: 1,
        bus: block.bus
      })
      break
    }
  }

  let outputFilename = `${name}-patch.json`
  fs.writeFileSync(outputFilename, JSON.stringify(origin))
  console.log(`Wrote ${outputFilename}`)

  let diffFilename = `${name}-patch-diff.json`
  fs.writeFileSync(diffFilename, JSON.stringify(diff))
  console.log(`Wrote ${diffFilename}`)
}
