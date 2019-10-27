var express = require('express')
var fs = require("fs");
var app = express();


app.use(express.static('public'))
app.get('/data', function (req, res, next) {
    fs.readFile('out_data.txt', 'utf-8', (err, data) => { 
    if (err) throw err; 
    res.send(data);
}) 

})

app.listen(3000, function () {
  console.log('N-body simulation at localhost:3000/index.html');
})