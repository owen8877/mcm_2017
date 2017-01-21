'use strict'

let file = './barrie.json'
let data = require(file)
const fs = require('fs')

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

fs.writeFile(`barrie.cinput`, tmp.join('\n'), function (err) {
  if (err) throw err;
  console.log(`Wrote barrie.cinput`)
})
