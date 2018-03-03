var coap = require('coap');
var hostprefix = "fd00::200:0:0:";
var index = 1;
var node = 4;
nodesaddresses = {};
var req = coap.request({host: hostprefix + node , pathname: '/sd6wsn/flow-mod', method: 'PUT' , retrySend: 5, query: 'operation=insert&flowid=1&ipv6src=fd00::200:0:0:4&ipv6dst=fd00::200:0:0:1&nhipaddr=fe80::0200:0:0:5&txpwr=3' });
	req.setOption('Max-Age', 65);
	req.on('response', function(res) {
  		res.pipe(process.stdout);
	})

	req.end();

