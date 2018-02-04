   var jsonArr = {node:"n3",nbr:{"n1":250,"n5":241,"n2":394}};
    console.log(jsonArr);
    console.log(Object.keys(jsonArr));
    console.log(jsonArr.nbr);


var chunk={id:"12",data:"123556",details:{"name":"alan","age":"12"}};
// chunk is already an object!

console.log(chunk.details);
// => {"name":"alan","age":"12"}

console.log(chunk.details.name);
//=> "alan"
