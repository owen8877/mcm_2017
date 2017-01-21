'use strict'

let map

let area = [[53.019033, -1.258524], [52.891518, -1.0879999]] //lat lng
let exclude = [{tri: true, content: [[52.940729, -1.258525], [52.917550, -1.178186], [52.889166, -1.258525]]}, {tri: true, content: [[52.891511, -1.172686], [52.947760, -1.085047], [52.891511, -1.085047]]}, {tri: true, content: [[53.019034, -1.085047], [52.947760, -1.085047], [53.019034, -1.151130]]}, {tri:true, content: [[52.891528, -1.179018], [52.912894,-1.2332621], [52.891528, -1.2332621]]}]
const scale = 3.14 * 6371000 / 180.0
const step = 300 / scale
let first = true

let latCount = Math.ceil((area[0][0] - area[1][0]) / step),
  lngCount = Math.ceil((area[1][1] - area[0][1]) / step)

const keySet = {
  '85': 'u',
  '79': 'o',
  '67': 'c',
  '87': 'w',
  '82': 'r'
}
const number = {
  '49': 1,
  '50': 2,
  '51': 3
}
let numberbuffer = 0

let time = (new Date()).getTime()
let bus = false

// unused - u 85, open - o 79, recreation - c 67, work - w 87, residential - r 82
document.onkeydown = e => {
  e = e || window.event
  let k = e.keyCode
  if (first) {
    if (e.keyCode == '32') {
      first = false
      next()
    } else {
      return
    }
  }
  for (let keyCode in keySet) {
    if (e.keyCode == keyCode) {
      data.push({x: -x, y: y - 1, type: keySet[keyCode], progress: numberbuffer, bus})
      bus = false
      //console.log(JSON.stringify(data.slice(-1)))
      console.log(`${parseInt((-x / latCount + y / (latCount*lngCount))*1000)/10}%`)
      dumpCount++;
      if (dumpCount > (latCount * lngCount / 100)) {
        dumpCount = 0
        console.save(data, `dump x-${x} y-${y}.json`)
      }
      next()
      break
    }
  }
  for (let n in number) {
    if (e.keyCode == n) {
      numberbuffer = number[n]
    }
  }
  if (e.keyCode == '46')
    back()
  if (e.keyCode == '66')
    bus = true
}

function initMap() {
  map = new google.maps.Map(document.getElementById('map'), {
    center: {lat: area[0][0], lng: area[0][1]},
    zoom: 19,
    mapTypeId: google.maps.MapTypeId.SATELLITE
  });
}

let x = 0, y = 0 // x - lat, y - lng
let data = []

const within = (lat, lng, a) => {
  return (lat <= a[0][0] && lng >= a[0][1]) && (lat >= a[1][0] && lng <= a[1][1])
}

const cross = (p, a, b) => {
  return (a[0]-p[0])*(b[1]-p[1])-(a[1]-p[1])*(b[0]-p[0])
}

const triwithin = (p, [a, b, c]) => {
  let ma = cross(p, a, b),
    mb = cross(p, b, c),
    mc = cross(p, c, a)
  return (ma>0 && mb>0 && mc>0) || (ma<0 && mb<0 && mc <0)
}

let dumpCount = 0

const next = () => {
  let startLat = area[0][0] + x * step
  let startLng = area[0][1] + y * step
  if (!within(startLat, startLng, area)) {
    x--;
    y = 0;
    //console.save(data, "dump" + (- x - 1) + ".json")
    let s = parseInt(((new Date()).getTime() - time) * (latCount - (-x)))
    time = (new Date()).getTime()
    //console.log(`Remaining ${s / 60} minutes ${s % 60} seconds`)
    startLat = area[0][0] + x * step
    startLng = area[0][1] + y * step
    if (!within(startLat, startLng, area)) {
      alert("end!")
      return
    }
  }

  while (true) {
    let excluded = false
    for (let ex of exclude) {
      if (ex.tri) {
        if (triwithin([startLat, startLng], ex.content)) {
          excluded = true
          break
        }
      } else {
        if (within(startLat, startLng, ex)) {
          excluded = true
          break
        }
      }
    }
    if (!excluded) break
    y++
    startLat = area[0][0] + x * step
    startLng = area[0][1] + y * step
    if (!within(startLat, startLng, area)) {
      x--;
      y = 0;
      //console.save(data, "dump" + (- x - 1) + ".json")
      let s = parseInt(((new Date()).getTime() - time) / (-x) * (latCount - (-x)))
      time = (new Date()).getTime()
      console.log(`Remaining ${s / 60} minutes ${s % 60} seconds`)
      startLat = area[0][0] + x * step
      startLng = area[0][1] + y * step
      if (!within(startLat, startLng, area)) {
        alert("end!")
        return
      }
    }
  }

  let coords = [
    {lat: startLat, lng: startLng},
    {lat: startLat + step, lng: startLng},
    {lat: startLat + step, lng: startLng + step},
    {lat: startLat, lng: startLng + step}
  ]

  !(new google.maps.Polygon({
    paths: coords,
    strokeColor: '#FF0000',
    strokeOpacity: 0.8,
    strokeWeight: 2,
    fillColor: '#FF0000',
    fillOpacity: 0.10
  })).setMap(map);

  map.setCenter({lat: startLat + step / 2, lng: startLng + step / 2})

  y++;
}

const back = () => {
  y--;
  data.pop();
  let startLat = area[0][0] + x * step
  let startLng = area[0][1] + y * step
  if (!within(startLat, startLng, area)) {
    x--
    y = Math.ceil((area[1][1] - area[0][1]) / step)
    startLat = area[0][0] + x * step
    startLng = area[0][1] + y * step
    if (!within(startLat, startLng, area)) {
      x = 0
      y = 0
    }
  }
  startLat = area[0][0] + x * step
  startLng = area[0][1] + y * step
  map.setCenter({lat: startLat + step / 2, lng: startLng + step / 2})
}


!(function(console){

console.save = function(data, filename){

    if(!data) {
        console.error('Console.save: No data')
        return;
    }

    if(!filename) filename = 'console.json'

    if(typeof data === "object"){
        data = JSON.stringify(data, undefined, 4)
    }

    var blob = new Blob([data], {type: 'text/json'}),
        e    = document.createEvent('MouseEvents'),
        a    = document.createElement('a')

    a.download = filename
    a.href = window.URL.createObjectURL(blob)
    a.dataset.downloadurl =  ['text/json', a.download, a.href].join(':')
    e.initMouseEvent('click', true, false, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null)
    a.dispatchEvent(e)
 }
})(console)
