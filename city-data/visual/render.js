'use strict'

const D3Node = require('d3-node')
const fs = require('fs')

const files = ['./Nottingham-test']

let data = []
let d3n = new D3Node()

for (let file of files) {
  data.push({name: file, content: require(file)})
}

for (let city of data) {
  let svg = d3n.createSVG()
    .attr("width", 1000)
    .attr("height", 1000)
    .append("g")

  let rects = svg.selectAll("rect")
    .data(city.content)
    .enter()
    .append("rect");

  rects
    .attr("x", d => {
      return d.x
    })
    .attr("y", d => {
      return d.y
    })
    .attr("width", d => {
      return 1
    })
    .attr("height", d => {
      return 1
    })

  let output = d3n.svgString()

  fs.writeFile(`${city.name}.svg`, output, function (err) {
    if (err) throw err;
    console.log('文件写入成功');
  })
}
