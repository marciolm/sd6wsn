const Graph = require('node-dijkstra')
const route = new Graph()
var coap = require('coap')
var req = {}
var hostprefix = "fd00::212:4b00:41e:"
var nexthopprefix = "fe80::212:4b00:41e:"
var rootnoderesp= {}
var flowpath = new Array()
const responsestimeout = 3000 //1500
var rootnode = "8e56"
var nodesaddresses = new Array()
var numnodes = 0
var lastflowid = 0
var graphnode = 0
var installevent=0
var responsecounter = 0
var getmetricstries = []

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
                        if(noderesp.nbr.hasOwnProperty("n" + rootnode))  // is root node in the path ?
                                rootnoderesp[noderesp.node] = noderesp.nbr.n8e56 //insert the value for n1
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
        //after all nodes responses, build the n1 link metric vector
        while(numnodes != graphnode) await sleep(responsestimeout)
        console.log("etx node 1",rootnoderesp)
        route.addNode("n" + rootnode, rootnoderesp)
        console.log("best paths to the root :")
        for(var i = 0; i < numnodes ; i++ ) {
                flowCalc(nodesaddresses[i],rootnode)
                await sleep(responsestimeout)
        }
        for(var i = 0; i < numnodes ; i++ ) {
                flowCalc(rootnode,nodesaddresses[i])
                await sleep(responsestimeout)
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


