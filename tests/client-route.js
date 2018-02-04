var coap = require('coap');
var hostprefix = "fd00::200:0:0:";
var index = 1;
var rootnode = 1;
nodesaddresses = {};
var req = coap.request({host: hostprefix + rootnode , pathname: '/sd6wsn/info-get/routes', query: 'index=' + index  , observe: false });
	
	req.setOption('Max-Age', 65);
	req.on('response', function(res) {
			var nodeaddr = JSON.parse(res.read());
			console.log(nodeaddr.dest.split(":")[5]);
			console.log("nodeaddr:",nodeaddr);
						console.log("nodeaddr.dest:",nodeaddr.dest);
			nodesaddresses.node = nodeaddr.dest.split(":")[5];
			console.log("nodesaddresses.node:",nodesaddresses.node);
			nodesaddresses.address = nodeaddr.dest ;
			console.log(nodesaddresses);
    })
req.end(); 
 
