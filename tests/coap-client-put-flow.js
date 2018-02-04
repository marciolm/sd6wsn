var coap        = require('coap')
var hostprefix = "fd00::200:0:0:";
var nodenumber = 2;

  var req = coap.request({host: hostprefix + nodenumber.toString(16) , pathname: '/sd6wsn/flow-mod', method: 'PUT' });
  req.write('insert?index=2&ipv6dst=”' + hostprefix + nodenumber +'”&nhmacaddr=”00124B000144F978”&txpwr=”3”');

  req.on('response', function(res) {
    res.pipe(process.stdout);
  })

  req.end();
