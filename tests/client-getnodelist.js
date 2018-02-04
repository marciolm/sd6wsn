var coap = require('coap');
var hostprefix = "fd00::200:0:0:";
var index = 1;
var rootnode = 1;
nodesaddresses = new Array();
var req = coap.request({host: hostprefix + rootnode , pathname: '/sd6wsn/node-mod', observe: false });
	
	req.setOption('Max-Age', 65);
	req.on('response', function(res) {
			var nodeaddr = JSON.parse(res.read());
			console.log(nodeaddr.nodes);
			nodesaddresses = nodeaddr.nodes.split(",");  // convert to a array
			console.log(nodesaddresses[0]); 
			console.log(Object.keys(nodesaddresses).length);  // matrix number of elements
//			console.log("rota numero 3: ", nodeaddr.routes[3]);
//			console.log("numero de rotas:",nodeaddr.nr);
    })
req.end(); 
 
