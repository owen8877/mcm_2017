'use strict'

const fs = require('fs')

let data = require('./barrie2.json')

/*data = data.map(d => {
    if (!(d.y>=11&&d.y<=54&&d.x>=8&&d.x<=40) && d.type == 'o')
        d.type = 'u'
    return d
})*/

for (let i = 0; i < data.length; ++i) {
    if (data[i].y>=35&&data[i].x>=25) {
        if (data[i].type == 'o') {
            if (data[i].progress == 1) {
                data[i].type = 'u'
                data[i].progress = 0
            } else {
                data[i].progress--;
            }
        }
    }
}

fs.writeFile(`barrie.json`, JSON.stringify(data), function (err) {
    if (err) throw err;
    console.log(`Wrote barrie.json`);
})
