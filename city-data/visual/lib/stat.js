'use strict'

const fs = require('fs')
const path = require('path')

let files = process.argv.slice(2)
let translate = {
  'u': 'unused land',
  'o': 'open land',
  'r': 'residential',
  'c': 'recreation and education',
  'w': 'working area'
}

for (let file of files) {
  let data = JSON.parse(fs.readFileSync(file))
  //console.log(data)

  console.log(`File '${file}': `)
  console.log(`Block number: ${data.length}`)

  let count = {}
  for (let block of data) {
    if (count[block.type] !== undefined) {
      count[block.type]++
    } else {
      count[block.type] = 1
    }
  }

  let develop = {}
  for (let block of data) {
    if (develop[block.type] !== undefined) {
      develop[block.type] += block.progress
    } else {
      develop[block.type] = block.progress
    }
  }

  console.log(' -> Blocks counts: ')
  let _count = {}
  for (let key in count) {
    _count[`${translate[key]}`] = count[key]
  }
  console.log(JSON.stringify(_count, 1, 2))
  console.log()
  console.log(' -> Blocks develop sum: ')
  let _develop = {}
  for (let key in develop) {
    _develop[`${translate[key]}`] = develop[key]
  }
  console.log(JSON.stringify(_develop, 1, 2))

  console.log('\n----------\n')
}
