var coap = require('coap')
var hostprefix = "fd00::200:0:0:"
var index = 1
var rootnode = 4
var nodesaddresses = {}

function objToString (obj) {
    var str = '';
    for (var p in obj) {
        if (obj.hasOwnProperty(p)) {
            str += String.fromCharCode(obj[p]);
        }
    }
    return str;
}

var req = coap.request({ method: 'GET' , host: hostprefix + rootnode , pathname: '/sd6wsn/packet-in', observe: true })
req.setOption('Max-Age', 130)
    .on('response', function (res) {
      res.on('data', function (res2) {
      console.log('tipo: ' + typeof(res2))
      resp = JSON.parse(objToString(res2))
      console.log("resp.packetin:",resp.packetin)
      console.log("resp.packetin.srcaddr:",resp.packetin.srcaddr)
      console.log('tipo resp: ' + typeof(resp))
      })   
    })
    .on('error', function (e) {
      console.log('Request error <' + e.toString())
    })
	req.end(); 

 function arraySort(numArray) {
	for(var i = 0; i < numArray.length - 1; i++) {
		var min = i
    	for(var j = i + 1; j < numArray.length; j++) {
        	if (numArray[j] < numArray[min]) min = j
    	}
    	if(min != i) {
        	var target = numArray[i]
        	numArray[i] = numArray[min]
        	numArray[min] = target
    	}
	}
	return numArray
}

