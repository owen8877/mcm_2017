'use strict'

const D3Node = require('d3-node')
const d3 = require('d3')
const fs = require('fs')
const path = require('path')
const MD = require('google-material-color')

const files = process.argv.slice(2)
const config = {
  scale: 10,
  padding: 0.05
}

let data = []

const colorPalette = (type, progress) => {
  const baseColor = {
    'u': 'Grey', //unused
    'o': 'Green', //open
    'c': 'Amber', //recreation
    'w': 'Indigo', //work
    'r': 'Deep Orange', //residential
    'wa': 'Grey', //debug use!
    null: 'Black'
  }
  const degreeConv = {
    1: '200',
    2: '300',
    3: '500',
    4: 'A400',
    0: '100',
    null: '100'
  }
  if (type == 'u')
    progress = 0
  return MD.palette[baseColor[type]][degreeConv[progress]]
}

for (let file of files) {
  let input = fs.readFileSync(path.resolve(process.env.PWD, file))
  let datum = JSON.parse(input)
  let name = path.basename(file, '.json')
  let border
  try {
    border = JSON.parse(fs.readFileSync(path.resolve(process.env.PWD, './border', `${name}-border.json`)))
  } catch (e) {
    console.log(e)
    border = null
  }
  datum.border = border
  data.push({name, content: datum})
}

const draw = (dataSet, width, height, subconfig = {typeOn: true, progressOn: true, busStation: false, hideBlock: false}) => {
  let d3n = new D3Node()

  let svg = d3n.createSVG()
    .attr("width", width * config.scale)
    .attr("height", height * config.scale)
    .attr("viewBox", `0 0 ${width} ${height}`)

  // draw background
  svg
    .append('rect')
    .attr('x', 0).attr('y', 0).attr('width', '100%').attr('height', '100%')
    .attr('fill', '#EEEEEE')

  // draw border
  let lineFunction = d3.svg.line()
    .x(function(d) { return d.x })
    .y(function(d) { return d.y })
    .interpolate("linear")

  if (dataSet.border)
    svg
      .append("path")
      .attr("d", lineFunction(dataSet.border))
      .attr("stroke", "blue")
      .attr("stroke-width", 0.4)
      .attr("fill", "none")

  // draw blocks
  svg
    .append('g')
    .selectAll("rect")
    .data(dataSet)
    .enter()
    .append("rect")
    .attr("x", d => {
      return d.y + config.padding
    })
    .attr("y", d => {
      return d.x + config.padding
    })
    .attr("width", d => {
      return 1 - 2 * config.padding
    })
    .attr("height", d => {
      return 1 - 2 * config.padding
    })
    .attr("fill", d => {
      if (subconfig.hideBlock)
        return colorPalette('wa', '2')
      return colorPalette(subconfig.typeOn ? d.type : 'wa', subconfig.progressOn ? d.progress : '3')
    })

  // draw bus station
  if (subconfig.busStation) {
    svg
      .append('g')
      .selectAll('circle')
      .data(dataSet.filter(item => item.bus))
      .enter()
      .append('circle')
      .attr('cx', d => d.y + 0.7)
      .attr('cy', d => d.x + 0.3)
      .attr('r', 0.1)
      .attr('fill', () => MD.get('Red'))
  }

  return d3n.svgString()
}

for (let city of data) {
  let width = 1 + city.content.reduce((acc, cur) => (acc > cur.y) ? acc : cur.y, 0),
    height = 1 + city.content.reduce((acc, cur) => (acc > cur.x) ? acc : cur.x, 0)
  console.log(width + " " + height)

  let seq = [
    {
      name: 'terrian',
      typeOn: false,
      progressOn: false,
      busStation: false,
      hideBlock: true
    },
    {
      name: 'development',
      typeOn: true,
      progressOn: true,
      busStation: false,
      hideBlock: false
    },
    {
      name: 'progressOnly',
      typeOn: false,
      progressOn: true,
      busStation: false,
      hideBlock: false
    },
    {
      name: 'typeOnly',
      typeOn: true,
      progressOn: false,
      busStation: false,
      hideBlock: false
    },
    {
      name: 'busStation',
      typeOn: false,
      progressOn: false,
      busStation: true,
      hideBlock: true
    }
  ]

  for (let subconfig of seq) {
    let output = draw(city.content, width, height, subconfig)

    let filenameconfig = subconfig.name
    fs.writeFileSync(`${city.name}-${filenameconfig}.svg`, output)
    console.log(`Wrote ${city.name}-${filenameconfig}.svg`)
  }
}
