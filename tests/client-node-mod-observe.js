var coap = require('coap')
var hostprefix = "fd00::200:0:0:"
var index = 1
var rootnode = 1
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

var req = coap.request({ method: 'GET' , host: hostprefix + rootnode , pathname: '/sd6wsn/node-mod', observe: true })
req.setOption('Max-Age', 130)
    .on('response', function (res) {
      res.on('data', function (res2) {
      console.log('tipo: ' + typeof(res2))
      nodeaddr = JSON.parse(objToString(res2))
      console.log("nodeaddr.nodes:",nodeaddr.nodes)
      nodesaddresses = nodeaddr.nodes.split(",")  // convert to a array
      console.log("Sorted Array:",arraySort(nodesaddresses)) //sort the array
      console.log('tipo nodeaddr: ' + typeof(nodeaddr))
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

