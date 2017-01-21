'use strict'

const D3Node = require('d3-node')
const fs = require('fs')
const MD = require('google-material-color')

const files = ['./Nottingham']
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
    0: '100',
    null: '100'
  }
  if (type == 'u')
    progress = 0
  return MD.palette[baseColor[type]][degreeConv[progress]]
}

for (let file of files) {
  data.push({name: file, content: require(file)})
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
      typeOn: false,
      progressOn: false,
      busStation: false,
      hideBlock: true
    },
    {
      typeOn: true,
      progressOn: true,
      busStation: false,
      hideBlock: false
    },
    {
      typeOn: false,
      progressOn: true,
      busStation: false,
      hideBlock: false
    },
    {
      typeOn: true,
      progressOn: false,
      busStation: false,
      hideBlock: false
    },
    {
      typeOn: false,
      progressOn: false,
      busStation: true,
      hideBlock: true
    }
  ]

  for (let subconfig of seq) {
    let output = draw(city.content, width, height, subconfig)

    let filenameconfig = Object.keys(subconfig).filter(key => subconfig[key]).join(' ')
    fs.writeFile(`${city.name} ${filenameconfig}.svg`, output, (err) => {
      if (err)
        throw err
      console.log(`Wrote ${city.name} ${filenameconfig}.svg`)
    })
  }
}
