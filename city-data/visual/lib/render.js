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

let ttttt = ['u', 'o', 'c', 'w', 'r']
let newData = []
for (let __x = 0; __x < 4; __x++) {
  for (let __y = 0; __y < 5; __y++) {
    newData.push({y:__x, x:__y, bus:false, type: ttttt[__y], progress: __x + 1})
  }
}
let tran = {
  'undeveloped': 'u',
  'open space': 'o',
  'recreation area': 'c',
  'working area': 'w',
  'residential area': 'r'
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
  datum.name = _name
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
      .attr('cx', d => d.y + 0.6)
      .attr('cy', d => d.x + 0.4)
      .attr('r', 0.2)
      .attr('fill', () => MD.get('Red'))
  }

  let textScale = 0.25
  // draw info
  svg
    .append("text")
    .attr("x", (width - 1.5) / textScale)
    .attr("y", 3.5 / textScale)
    .attr("text-anchor", "end")
    .style("font-size", `${64 * textScale}px`)
    .attr('fill', MD.get("Grey", 800))
    .attr("transform", `scale(${textScale}, ${textScale})`)
    //.text(dataSet.borderExtra.name)
    .text(dataSet.name)

  /*svg
    .append("text")
    .attr("x", width / textScale)
    .attr("y", 5 / textScale)
    .attr("text-anchor", "end")
    .style("font-size", `${40 * textScale}px`)
    .attr("transform", `scale(${textScale}, ${textScale})`)
    //.text(dataSet.borderExtra.name)
    .text(seqName)*/

  let fixx = width-7.5, fixy=height-7.5
  // legend
  svg
    .append('g')
    .selectAll("rect")
    .data(newData)
    .enter()
    .append("rect")
    .attr("x", d => {
      return 2 * d.y + config.blockPadding
    })
    .attr("y", d => {
      return 2 * d.x + config.blockPadding
    })
    .attr("width", d => {
      return 2* (1 - 2 * config.blockPadding)
    })
    .attr("height", d => {
      return 2 * (1 - 2 * config.blockPadding)
    })
    .attr("fill", d => {
      //console.log('lol')
      return colorPalette(typeOn ? d.type : 'wa', progressOn ? d.progress : '3')
    })
    .attr('transform', `translate(${fixx}, ${fixy})`)

  let ssscale = 0.14
  let keys = Object.keys(tran)
  for (let i = 0; i < keys.length; i++) {
    let lx = (fixx-0.5) / ssscale, ly = (2 * i + 1 + fixy) / ssscale
    svg
      .append("text")
      .attr("x", lx)
      .attr("y", ly)
      .attr("text-anchor", "end")
      .attr('fill', MD.get("Grey", 800))
      .style("font-size", `${80 * ssscale}px`)
      .attr("transform", `scale(${ssscale}, ${ssscale}) rotate(0, ${lx}, ${ly})`)
      //.text(dataSet.borderExtra.name)
      .text(keys[i])
  }

  for (let i = 1; i < 5; i++) {
    let lx = (2 * i - 1.5 + fixx) / ssscale, ly = (fixy - 0.5) / ssscale
    svg
      .append("text")
      .attr("x", lx)
      .attr("y", ly)
      .attr("text-anchor", "mid")
      .attr('fill', MD.get("Grey", 700))
      .style("font-size", `${80 * ssscale}px`)
      .attr("transform", `scale(${ssscale}, ${ssscale}) rotate(0, ${lx}, ${ly})`)
      //.text(dataSet.borderExtra.name)
      .text(i)
  }

  let lx = (-0.5 + fixx) / ssscale, ly = (fixy - 3) / ssscale
  svg
    .append("text")
    .attr("x", lx)
    .attr("y", ly)
    .attr("text-anchor", "mid")
    .attr('fill', MD.get("Grey", 800))
    .style("font-size", `${120 * ssscale}px`)
    .attr("transform", `scale(${ssscale}, ${ssscale}) rotate(0, ${lx}, ${ly})`)
    //.text(dataSet.borderExtra.name)
    .text("Legend")

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
