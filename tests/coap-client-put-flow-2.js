var coap        = require('coap')
var hostprefix = "fd00::200:0:0:";


 function installflow(node,nhop) {
  var reqput = coap.request({host: hostprefix + node.toString(16) , pathname: '/sd6wsn/flow-mod', method: 'PUT' });
  reqput.write('insert?index=2&ipv6dst=”' + hostprefix + node +'”&nhmacaddr=”' + hostprefix + nhop +'”&txpwr=”3”');

  reqput.on('response', function(res) {
    res.pipe(process.stdout);
  })

  reqput.end();
}
installflow(5,6); 
