var coap = require('coap');
var hostprefix = "fd00::200:0:0:";
var index = 1;
var rootnode = 1;
nodesaddresses = {};
var req = coap.request({host: hostprefix + rootnode , pathname: '/sd6wsn/node-mod', observe: false });
	
	req.setOption('Max-Age', 65);
	req.on('response', function(res) {
			var nodeaddr = JSON.parse(res.read());
			console.log(nodeaddr.nodes);

    })
req.end(); 
 
