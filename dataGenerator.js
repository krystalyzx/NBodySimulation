var fs = require('fs');
data = "";
for(i = 0;i< 10000;i++){
    data = data + (Math.random()*5).toString() + " ";
    data = data + (Math.random()*5).toString() + " ";
    data = data + "2000\n";
}
fs.writeFile('data.txt', data, function (err) {
    if (err) throw err;
    console.log('Saved!');
  });