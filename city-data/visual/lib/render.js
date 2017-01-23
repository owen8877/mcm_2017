'use strict'

const D3Node = require('d3-node')
const d3 = require('d3')
const fs = require('fs')
const path = require('path')
const MD = require('google-material-color')

const files = process.argv.slice(2)
const config = {
  scale: 10,
  blockPadding: 0.05,
  margin: 10
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
  let input = fs.readFileSync(file)
  let datum = JSON.parse(input)
  let name = path.basename(file, '.json')
  let border, borderExtra
  let _name = name.split('-')[0]
  let _diff = name.split('-').includes("diff")
  try {
    border = require(`../border/${_name}-border.json`)
    borderExtra = require(`../border/${_name}-border-extra.json`)
  } catch (e) {
    console.log(`Cannot find terrian ../border/${_name}-border-extra.json!`)
    //border = null
    exit(2)
  }

  let {width, height} = borderExtra.wh
  /*** caution!!! filter x<0 || y<0 points*/
  datum = datum.filter(d => d.x>=0 && d.y>=0 && d.x<height && d.y<width)
  /*****/

  datum.border = border
  datum.diff = _diff
  datum.borderExtra = borderExtra
  data.push({name, content: datum})
}

//const borderProcess = d3.svg.line().x(d => d.x).y(d => d.y).interpolate("linear")

const draw = (dataSet, width, height, {typeOn = true, progressOn = true, busStation = false, hideBlock = false, terrian = false, seqName}, filename) => {
  let d3n = new D3Node()

  let svg = d3n.createSVG()
    .attr("width", (width + config.margin) * config.scale)
    .attr("height", (height + config.margin) * config.scale)
    .attr("viewBox", `${-config.margin / 2} ${-config.margin / 2} ${width + config.margin} ${height + config.margin}`)

  // draw background
  svg
    .append('rect')
    .attr('x', -config.margin / 2)
    .attr('y', -config.margin / 2)
    .attr('width', width + config.margin)
    .attr('height', height + config.margin)
    .attr('fill', MD.get('Grey', '50'))

  // draw blocks
  svg
    .append('g')
    .selectAll("rect")
    .data(dataSet)
    .enter()
    .append("rect")
    .attr("x", d => {
      return d.y + config.blockPadding
    })
    .attr("y", d => {
      return d.x + config.blockPadding
    })
    .attr("width", d => {
      return 1 - 2 * config.blockPadding
    })
    .attr("height", d => {
      return 1 - 2 * config.blockPadding
    })
    .attr("fill", d => {
      if (dataSet.borderExtra.diff && busStation && !d.bus)
        return "none"
      if (hideBlock)
        return colorPalette('wa', '2')
      return colorPalette(typeOn ? d.type : 'wa', progressOn ? d.progress : '3')
    })

  // draw border
  if (dataSet.border) {
    //console.log(dataSet.borderExtra.lnglat)
    //geoMercator()
    //geoOrthographic()
    let projection = d3.geoMercator().scale(dataSet.borderExtra.scale).center(dataSet.borderExtra.lnglat).translate([0, 0])
    let geoPath = d3.geoPath(projection)
    //console.log(dataSet.diff)
    svg
      .append('path')
      .datum(dataSet.border)
      .attr('d', geoPath)
      .attr("stroke", terrian ? MD.get('Blue', '900') : MD.get('Grey'))
      .attr("stroke-width", 0.2)
      .attr("stroke-opacity", terrian ? 0.5 : (dataSet.diff ? 0.8 : 0.0))
      .attr("fill", terrian ? MD.get('Blue') : MD.get('Grey'))
      .attr("fill-opacity", terrian ? 0.5 : (dataSet.diff ? 0.1 : 0.0))
      .attr("transform", `scale(${1 / Math.cos(3.14 / 180 * dataSet.borderExtra.lnglat[1])}, 1)`)
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

  let textScale = 0.25
  // draw info
  svg
    .append("text")
    .attr("x", width / textScale)
    .attr("y", 0 / textScale)
    .attr("text-anchor", "end")
    .style("font-size", `${64 * textScale}px`)
    .attr("transform", `scale(${textScale}, ${textScale})`)
    //.text(dataSet.borderExtra.name)
    .text(filename)

  svg
    .append("text")
    .attr("x", width / textScale)
    .attr("y", 5 / textScale)
    .attr("text-anchor", "end")
    .style("font-size", `${40 * textScale}px`)
    .attr("transform", `scale(${textScale}, ${textScale})`)
    //.text(dataSet.borderExtra.name)
    .text(seqName)

  return d3n.svgString()
}

for (let city of data) {
  let {width, height} = city.content.borderExtra.wh

  let seq = [
    {
      seqName: 'terrian',
      typeOn: false,
      progressOn: false,
      hideBlock: true,
      terrian: true
    },
    {
      seqName: 'development',
    },
    {
      seqName: 'progressOnly',
      typeOn: false,
    },
    {
      seqName: 'typeOnly',
      progressOn: false,
    },
    {
      seqName: 'busStation',
      typeOn: false,
      progressOn: false,
      busStation: true,
      hideBlock: true
    }
  ]

  for (let subconfig of seq) {
    let output = draw(city.content, width, height, subconfig, city.name)

    let filenameconfig = subconfig.seqName
    fs.writeFileSync(`${city.name}-${filenameconfig}.svg`, output)
    console.log(`Wrote ${city.name}-${filenameconfig}.svg`)
  }
}
