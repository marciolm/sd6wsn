var coap = require('coap')
var hostprefix = "fd00::200:0:0:"
var index = 1
var rootnode = 4
var nodesaddresses = {}
var req = coap.request({ method: 'GET' , host: hostprefix + rootnode , pathname: '/sd6wsn/info-get/rssi', observe: true })
	
	req.setOption('Max-Age', 130)
	req.on('response',function(res){
    console.log('------------Response--------------')
    console.log("Status Code:"+res.code)
    res.options.forEach(function(item){
        console.log('Option Name:'+item.name+' value:'+item.value)
    })
    console.log("Body:"+res.payload)
})

	req.on('data',function(d){
    console.log("---------New data arrived--------")
    console.log("Payload:"+d.payload)
})
	req.end(); 
 
