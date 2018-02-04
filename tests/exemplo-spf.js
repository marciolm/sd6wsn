const Graph = require('node-dijkstra')

const route = new Graph()
var noderesp = '';
var caminho = new Array();

//route.addNode('A', { B:1 })
//route.addNode('B', { A:1, C:2, D: 4 })
//route.addNode('C', { B:2, D:1 })
//route.addNode('D', { C:1, B:4 })
route.addNode("n3", {"n1":383,"n5":210,"n2":180})
route.addNode("n9", {"n6":437,"n8":183})
route.addNode("n8", {"n9":338,"n5":287,"n7":301})
route.addNode("n4", {"n5":265,"n2":544,"n7":173})
route.addNode("n6", {"n1":291,"n9":364,"n5":235})
route.addNode("n2", {"n3":284,"n4":352})
route.addNode("n5", {"n6":386,"n3":182,"n4":284,"n8":324})
route.addNode("n7", {"n4":533,"n8":398})
route.addNode("n1", {"n3":383,"n6":291})
noderesp = {"n6":386,"n3":182,"n4":284,"n8":324}
caminho[7] = route.path("n1", "n7")
//console.log("size:",Object.keys(caminho).lenght);
console.log("caminho:",caminho[7]);

console.log(route.path("n1", "n2")) 
console.log(route.path("n2", "n1"))
console.log(route.path('n1', 'n3'))
console.log(route.path("n3", "n1")) 
console.log(route.path('n1', 'n4')) 
console.log(route.path('n1', 'n5')) 
console.log(route.path('n1', 'n6')) 
console.log(route.path('n1', 'n7')) 
console.log(route.path('n1', 'n8'))
console.log(route.path('n8', 'n1'))
noderesp = {"n6":386,"n3":182,"n4":284,"n8":324}
if(noderesp.hasOwnProperty("n8")) console.log(noderesp.n8); 
noderesp.n9=100;
console.log(noderesp)

