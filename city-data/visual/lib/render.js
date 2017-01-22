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
    0: '200',
    null: '200'
  }
  if (type == 'u')
    progress = 0
  return MD.palette[baseColor[type]][degreeConv[progress]]
}

for (let file of files) {
  let input = fs.readFileSync(path.resolve(process.env.PWD, file))
  let datum = JSON.parse(input)
  let name = path.basename(file, '.json')
  let border, borderExtra
  try {
    let _name = name.split('-')[0]
    //border = JSON.parse(fs.readFileSync(path.resolve('../border', `${_name}-border.json`)))
    border = require(`../border/${_name}-border.json`)
    borderExtra = require(`../border/${_name}-border-extra.json`)
  } catch (e) {
    console.log('Cannot find terrian!')
    border = null
  }
  datum.border = border
  datum.borderExtra = borderExtra
  data.push({name, content: datum})
}

//const borderProcess = d3.svg.line().x(d => d.x).y(d => d.y).interpolate("linear")

const draw = (dataSet, width, height, {typeOn = true, progressOn = true, busStation = false, hideBlock = false, terrian = false}) => {
  let d3n = new D3Node()

  let svg = d3n.createSVG()
    .attr("width", width * config.scale)
    .attr("height", height * config.scale)
    .attr("viewBox", `0 0 ${width} ${height}`)

  // draw background
  svg
    .append('rect')
    .attr('x', 0).attr('y', 0).attr('width', '100%').attr('height', '100%')
    .attr('fill', MD.get('Grey', '50'))

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
      if (hideBlock)
        return colorPalette('wa', '2')
      return colorPalette(typeOn ? d.type : 'wa', progressOn ? d.progress : '3')
    })

  // draw border
  if (dataSet.border) {
    //let projection = d3.geoMercator().center(dataSet.borderExtra.latlng)
    //let path = d3.geoPath(projection)
    svg
      .selectAll('path')
      .data(dataSet.border)
      .enter()
      .append('path')
      .attr('d', path)
      /*.append("path")
      .attr("d", borderProcess(dataSet.border))*/
      .attr("stroke", terrian ? MD.get('Blue', '900') : MD.get('Grey'))
      .attr("stroke-width", 0.2)
      .attr("stroke-opacity", terrian ? 0.5 : 0.3)
      .attr("fill", terrian ? MD.get('Blue') : MD.get('Grey'))
      .attr("fill-opacity", terrian ? 0.5 : 0.1)
  }

  // draw bus station
  if (busStation) {
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

  /*if (city.content.border) {
    width = 1 + city.content.border.reduce((acc, cur) => (acc > cur.x) ? acc : cur.x, 0),
    height = 1 + city.content.border.reduce((acc, cur) => (acc > cur.y) ? acc : cur.y, 0)
  }*/
  console.log(width + " " + height)

  let seq = [
    {
      name: 'terrian',
      typeOn: false,
      progressOn: false,
      hideBlock: true,
      terrian: true
    },
    {
      name: 'development',
    },
    {
      name: 'progressOnly',
      typeOn: false,
    },
    {
      name: 'typeOnly',
      progressOn: false,
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
