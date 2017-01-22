'use strict'

const fs = require('fs')

let data = require('./Nottingham2.json')

/*data = data.map(d => {
    if (!(d.y>=11&&d.y<=54&&d.x>=8&&d.x<=40) && d.type == 'o')
        d.type = 'u'
    return d
})*/

for (let i = 0; i < data.length; ++i) {
    if (!(data[i].y>=11&&data[i].y<=54&&data[i].x>=8&&data[i].x<=40) && data[i].type == 'o')
        data[i].type = 'u'
}

fs.writeFile(`Nottingham.json`, JSON.stringify(data), function (err) {
    if (err) throw err;
    console.log(`Wrote Nottingham.json`);
})
