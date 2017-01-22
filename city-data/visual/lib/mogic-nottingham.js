'use strict'

const fs = require('fs')

let data = require('../nottingham-orig.json')

for (let i = 0; i < data.length; ++i) {
    if (!(data[i].y>=11&&data[i].y<=54&&data[i].x>=8&&data[i].x<=40)) {
        if (data[i].type == 'o') {
            if (data[i].progress == 1) {
                data[i].type = 'u'
                data[i].progress = 0
            } else {
                data[i].progress--
            }
        }
    }
}

fs.writeFile(`nottingham.json`, JSON.stringify(data), function (err) {
    if (err) throw err;
    console.log(`Wrote nottingham.json`);
})
