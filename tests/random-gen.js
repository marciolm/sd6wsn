var origens = []	
var destinos = []	

for(var j = 0; j < 20 ; j++ ){	
        var randomnumber = getRandomInt(2,26)
	while(origens.indexOf(randomnumber) != "-1" ){
		randomnumber = getRandomInt(2, 26)
	}
	origens[j]=randomnumber
	randomnumber = getRandomInt(2,26)
	while(randomnumber == origens[j])
		randomnumber = getRandomInt(2, 26) 
	destinos[j]=randomnumber
}

console.log(origens)
console.log(destinos)

function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

