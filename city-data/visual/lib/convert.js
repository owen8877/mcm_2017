'use strict'
const fs = require('fs')
const path = require('path')

let files = process.argv.slice(2)

for (let file of files) {
  let input = fs.readFileSync(path.resolve(process.env.PWD, file))
  let data = JSON.parse(input)
  let X = 1 + data.reduce((acc, cur) => {
    return (acc > cur.x) ? acc : cur.x
  }, 0)
  let Y = 1 + data.reduce((acc, cur) => {
    return (acc > cur.y) ? acc : cur.y
  }, 0)

  console.log(`${X} ${Y}`)

  let tmp = []
  //tmp.push(`${X} ${Y}`)

  const typeConv = {
    'r': 0,
    'w': 1,
    'c': 2,
    'o': 3,
    'u': 4
  }

  for (let d of data) {
    tmp.push(`${d.x} ${d.y} ${d.progress} ${typeConv[d.type]} ${d.bus ? 'T' : 'F'}`)
  }

  fs.writeFileSync(`cinput/${path.basename(file, '.json')}.cinput`, tmp.join('\n'))
  console.log(`Wrote cinput/${path.basename(file, '.json')}.cinput`)
}

