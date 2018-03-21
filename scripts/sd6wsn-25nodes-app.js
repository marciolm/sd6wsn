const Graph = require('node-dijkstra')
const route = new Graph()
var coap = require('coap')
var req = {}
var hostprefix = "fd00::200:0:0:"
var nexthopprefix = "fe80::200:0:0:"
var nodeoneresp= {}
var flowpath = new Array()
const responsestimeout = 3000 //1500
var rootnode = 1
var nodesaddresses = new Array()
var numnodes = 0
var lastflowid = 0
var graphnode = 0
var installevent=0
var responsecounter = 0
var getmetricstries = []

var lista1 = [24,18,25,5,24,4,18,18,12,21,23,4,25,3,12,15,6,9,23,16,20,16,24,24,4,23,18,9,4,14,21,6,19,17,20,21,11,18,21,20,23,11,3,8,13,7,11,15,15,6,5,14,17,9,23,16,13,21,25,19]
var lista2 = [16,20,24,18,25,16,19,15,9,5,14,12,4,4,4,10,16,18,10,7,7,4,18,18,26,17,23,21,20,6,12,4,18,18,7,4,26,22,20,23,9,12,24,6,8,9,12,3,21,15,23,12,17,25,11,13,13,17,10,18]
var lista3 = [16,5,6,18,18,17,13,14,13,19,7,24,6,8,3,19,5,20,12,12,4,11,13,11,9,7,22,8,15,20,20,23,7,24,10,2,9,11,21,2,6,7,14,15,16,14,20,11,25,11,21,4,13,15,23,6,8,11,3,7]
var lista4 = [5,23,16,15,23,9,2,9,6,17,3,20,19,24,25,16,25,5,10,16,25,7,14,11,22,14,4,9,10,15,19,18,23,22,23,26,5,23,14,4,23,18,26,25,11,23,2,10,24,4,25,11,22,2,21,16,21,9,18,7]
var lista5 = [6,8,11,7,13,21,26,6,9,14,3,8,2,4,7,18,25,3,25,7,4,15,23,15,20,11,15,8,10,18,21,23,4,16,21,24,12,20,16,17,18,9,3,16,23,10,13,18,9,4,18,24,21,15,23,24,8,20,3,17]
var lista6 = [7,25,4,8,11,7,15,18,24,9,25,17,3,14,6,23,15,8,14,10,4,17,6,26,11,26,14,5,17,9,18,5,12,4,7,19,17,18,9,22,5,25,15,23,16,17,10,2,24,25,12,15,17,19,14,17,8,22,8,23]
var lista7 = [26,6,16,14,9,26,2,13,25,7,23,3,24,5,6,20,8,20,6,17,6,10,7,16,7,6,19,15,22,18,4,5,24,25,13,3,8,20,8,14,7,23,9,8,3,25,21,22,8,5,16,6,12,13,11,8,4,10,4,16]
var lista8 = [4,20,17,17,11,10,24,6,24,17,24,22,6,15,5,16,7,8,20,11,24,10,15,3,14,20,26,25,8,24,7,2,5,24,13,16,22,18,17,6,8,3,5,5,15,19,23,18,15,12,18,8,4,19,15,21,21,23,19,3]
var lista9 = [12,5,12,17,18,13,8,22,7,6,13,18,14,21,15,16,15,13,19,2,7,21,11,17,14,14,18,5,15,8,7,18,16,26,4,16,18,4,14,12,23,11,19,3,19,21,9,9,25,15,7,14,2,19,4,7,5,22,25,13]
var lista10 = [9,16,12,8,21,3,9,9,25,12,18,20,15,19,2,14,25,22,11,7,23,8,23,17,25,12,3,22,25,6,21,15,2,11,18,18,21,5,16,20,16,3,17,24,14,24,14,8,19,9,17,16,10,20,13,25,11,22,13,11]
var lista11 = [16,6,8,20,21,3,20,13,26,15,8,16,20,7,12,21,15,22,5,4,13,6,4,6,6,16,19,21,15,12,21,24,13,17,23,26,16,11,21,15,22,19,20,19,7,3,4,11,9,9,25,26,16,15,14,12,11,4,26,5]
var lista12 = [22,13,11,7,20,2,25,6,14,24,12,18,23,7,2,5,6,16,12,3,13,21,14,23,17,4,11,17,17,22,12,23,19,7,9,19,10,13,11,11,9,14,13,3,4,10,2,15,6,4,19,11,2,10,20,12,19,17,24,11]
var lista13 = [25,10,12,9,19,23,7,21,16,21,5,9,18,17,11,4,25,18,13,4,24,23,11,3,26,10,17,6,20,14,8,2,6,11,24,8,20,7,8,16,17,3,14,13,8,6,24,19,18,12,3,17,3,11,5,9,15,19,21,7]
var lista14 = [11,15,14,24,14,25,20,5,8,14,12,11,24,9,24,18,12,8,19,20,18,15,7,5,19,23,3,3,23,3,17,18,13,2,7,10,22,22,4,21,24,11,11,10,5,10,20,18,18,13,4,22,14,14,21,21,25,19,3,5]
var lista15 = [10,9,21,16,17,8,8,16,26,8,14,14,24,18,17,22,12,9,21,23,9,24,7,16,3,7,20,21,20,12,10,19,24,13,16,23,6,8,9,15,16,16,18,20,19,22,18,16,8,12,7,22,25,12,22,7,26,23,11,6]
var lista16 = [22,23,2,26,23,7,6,2,6,16,6,17,5,17,25,23,19,15,25,13,26,21,20,14,4,22,14,5,26,9,11,8,19,15,15,23,23,20,21,21,14,6,17,6,20,14,15,9,11,20,23,17,12,5,5,8,15,21,24,19]
var lista17 = [10,23,10,4,25,22,25,20,13,22,23,16,3,25,10,5,4,6,8,11,23,6,20,12,12,23,24,7,7,21,14,3,10,7,2,2,24,14,6,23,25,8,15,4,26,20,12,21,6,13,19,18,19,16,15,24,14,4,6,20]
var lista18 = [25,24,23,14,26,20,14,16,11,24,19,4,23,14,12,5,20,19,8,19,15,6,9,8,3,7,3,11,17,19,5,8,5,25,9,9,19,26,10,7,25,25,16,24,4,22,6,4,17,3,19,12,16,12,6,26,4,25,25,14]
var lista19 = [18,13,16,18,5,24,8,24,2,15,16,19,7,10,14,26,13,26,15,25,11,18,22,13,5,10,23,22,8,18,24,23,9,19,16,24,25,19,3,3,14,4,21,13,10,4,24,2,16,7,5,17,18,14,21,21,22,18,11,4]
var lista20 = [24,10,21,12,2,24,19,5,25,16,24,7,6,11,16,13,4,10,3,12,23,14,4,16,6,15,21,4,23,26,19,5,18,9,19,4,20,11,7,3,20,20,24,26,2,12,22,26,9,9,23,9,18,15,3,23,25,18,16,11]

coap.parameters
.exchangeLifetime = 45
//.ackTimeout = 5
//.maxRetransmit = 300
//.ackRandomFactor= 90

function getNodes() {
	var req = coap.request({host: hostprefix + rootnode , pathname: '/sd6wsn/node-mod' , observe: false })
	req.setOption('Max-Age', 130)
	req.on('response', function(res) {
		res.on('data', function (res2) {
			var nodeaddr = JSON.parse(objToString(res2))
			if(nodeaddr.nodes == '') getNodes() 
			else {
				console.log("nodeaddr.nodes:",nodeaddr.nodes)
				nodesaddresses = nodeaddr.nodes.split(",")  // convert to a array
				console.log("nodesaddresses:",nodesaddresses)
				console.log("Sorted Array:",arraySort(nodesaddresses)) //sort the array
				numnodes = Object.keys(nodesaddresses).length  // lenght of array
				treeCalc() // yield the treecalc after the number of nodes definition
				for(var i = 0; i < numnodes ; i++ ){
					getmetricstries[nodesaddresses[i]] = 0
					getMetrics(nodesaddresses[i])
				}
			}
		})
	})
	req.on('error', function (err) {
		console.log("6LBR unreachable")
		process.exit(0)
	}) 
	req.end() 
}
        
function getMetrics(nodeaddress) {
	console.log("nodeaddress:",nodeaddress)
	req = coap.request({host: hostprefix + nodeaddress, pathname: '/sd6wsn/info-get/nbr-etx', observe: false , retrySend: 10  })
	//req.setOption('Max-Age', 65)
	req.on('response', function(res) {
		res.on('data', function (res2) {
			var noderesp = JSON.parse(objToString(res2))
			console.log("noderesp.node:",noderesp.node,noderesp.nbr) //node, link metric for neighbors
			route.addNode(noderesp.node, noderesp.nbr) //insert the node on Graph 
			graphnode ++
			console.log("node added:",graphnode,"/",numnodes)
		})	
	})
	req.on('error', function (err) {
        	console.log("error on getmetrics " + nodeaddress + " tries: " + getmetricstries[nodeaddress])
		if(getmetricstries[nodeaddress] < 3) {
			getmetricstries[nodeaddress]++
			getMetrics(nodeaddress) //try one more time
		}
    		//console.log(err)
	}) 
	req.end()
}

async function treeCalc() {

	while(numnodes != graphnode) await sleep(responsestimeout)	
	console.log("best path:")
	for(var i = 0; i < lista1.length ; i++ ) {  
		flowCalc(lista1[i].toString(16),lista2[i].toString(16))
		await sleep(responsestimeout)
	var req = coap.request({host: hostprefix + lista1[i].toString(16) , pathname: '/test/udpdest', method: 'PUT' , retrySend: 5, query: 'index=' + lista2[i]});
	req.setOption('Max-Age', 65);
	req.on('response', function(res) {
  		res.pipe(process.stdout);
	})

	req.end();
	}
}

function flowCalc(srcnode, dstnode) {
	var flowpathsize = 0
	var ipv6srctemp = 0
	var ipv6dsttemp= 0
	lastflowid ++
	var flowidtemp = lastflowid
	flowpath[flowidtemp] = route.path("n" + srcnode, "n" + dstnode) //calc shortest path
	console.log(flowpath[flowidtemp])  // [ 'n5', 'n6', 'n1' ]
	for(var prop in flowpath[flowidtemp]) flowpathsize ++   // count number of nodes in path
	for(var nodeinpath = 0; nodeinpath < flowpathsize; nodeinpath++) {
	ipv6srctemp = hostprefix + flowpath[flowidtemp][0].slice(1)
		if(nodeinpath < flowpathsize - 1){ // do until the penultimate node
			var installnodetemp = hostprefix + flowpath[flowidtemp][nodeinpath].slice(1) 
			var nxhoptemp = nexthopprefix + flowpath[flowidtemp][nodeinpath+1].slice(1) //next node of flow 
			if(flowpath[flowidtemp][flowpathsize-1].slice(1) == 1) ipv6dsttemp="fd00::200:0:0:1"   //if the last node is the root, change the dst to root
			else ipv6dsttemp = hostprefix + flowpath[flowidtemp][flowpathsize-1].slice(1) // else, dst is the last node of path
			installevent ++	     	
			console.log("installnode:" + installnodetemp + " ipvsrc:"  + ipv6srctemp + " ipv6dst:" + ipv6dsttemp + " nxhop:" + nxhoptemp + " installevent:" + installevent + " flowid:" + flowidtemp)
			flowEntryInstall(installnodetemp,flowidtemp,ipv6srctemp,ipv6dsttemp,nxhoptemp) 
		}
	}
}

function flowEntryInstall(installnode,flowid,ipv6src,ipv6dst,nxhop) {
	var req = coap.request({host: installnode , pathname: '/sd6wsn/flow-mod', method: 'PUT' , query: 'operation=insert&flowid=' + flowid + '&ipv6src=' + ipv6src + '&ipv6dst=' + ipv6dst +'&action=0' + '&nhipaddr=' +  nxhop +'&txpwr=3', retrySend: 5  })
	//console.log("installnode=",installnode)
	req.setOption('Max-Age', 65)
	req.on('response', function(res) {
  		responsecounter ++
  		console.log("response " + responsecounter + " from node: " + installnode)
	})
	req.on('error', function (err) {
    		//console.log(err)
    		console.log("retrying flow install on node: " + installnode)
		flowEntryInstall(installnode,flowid,ipv6src,ipv6dst,nxhop) //try one more time
	}) 
	req.end()
}


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

function sleep(ms){
    return new Promise(resolve=>{
        setTimeout(resolve,ms)
    })
}

function objToString (obj) {
    var str = '';
    for (var p in obj) {
        if (obj.hasOwnProperty(p)) {
            str += String.fromCharCode(obj[p]);
        }
    }
    return str;
}

getNodes()


