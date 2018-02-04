var http = require('http');
var loc = "C:/Users/a/Desktop/node.m test";

http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/html'});
  var exec = require('child_process').exec,
    child;
    child = exec('"C:/Program Files/Wolfram Research/Mathematica/10.0/Math" -noprompt -script '+loc,
      function (error, stdout, stderr) {
        console.log('stderr: ' + stderr);
        res.end(
          (stdout).replace(/\\n/g,"\n").slice(1,-3)
        );
        if (error !== null) {
          console.log('exec error: ' + error);
        }
    });
}).listen(1337, "127.0.0.1");

console.log('Server running at http://127.0.0.1:1337/');
