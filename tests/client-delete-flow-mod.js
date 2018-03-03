var coap = require('coap');
var hostprefix = "fd00::200:0:0:";
var index = 1;
var node = 4;
nodesaddresses = {};
var req = coap.request({host: hostprefix + node , pathname: '/sd6wsn/flow-mod', method: 'PUT' , retrySend: 5, query: 'operation=delete&flowid=1' });
	req.setOption('Max-Age', 65);
	req.on('response', function(res) {
  		res.pipe(process.stdout);
	})

	req.end();

