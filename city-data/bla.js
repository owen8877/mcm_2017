var rect = d3.select('svg').append('rect').attr('width', 1).attr('height', 1).attr('stroke', 1).attr('fiil', '#FFFFFF')
let box = []
let x = 0, y = 0
document.onkeydown = (e) => {handle(e.keyCode)}
let handle = k => {
    console.log(k)
    if (k == '32') {
        box.push({x, y})
        d3.select('svg').append('rect').attr('width', 1).attr('height', 1).attr('stroke', 1).attr('x', y).attr('y', x).attr('fiil', '#FFFFFF')
    } else if (k == '38') {
        x--
    } else if (k == '40') {
        x++
    } else if (k == '37') {
        y--
    } else if (k == '39') {
        y++
    }
    rect.attr('y', x)
    rect.attr('x', y)
}
