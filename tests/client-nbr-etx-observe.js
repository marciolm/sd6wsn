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

var req = coap.request({ method: 'GET' , host: hostprefix + rootnode , pathname: '/sd6wsn/info-get/nbr-etx', observe: true })
req.setOption('Max-Age', 130)
    .on('response', function (res) {
      res.on('data', function (res2) {
      console.log('tipo: ' + typeof(res2))
      teste = JSON.parse(objToString(res2))
      console.log('tipo teste: ' + typeof(teste))
         console.log('resposta: ' , teste.nbr )
      })   
    })
    .on('error', function (e) {
      console.log('Request error <' + e.toString())
    })
	req.end(); 
 
