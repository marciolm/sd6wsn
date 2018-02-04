var callback = console.log;
var jsonArr =[ 'n6', 'n5', 'n9', 'n1' ];
function traverse(obj) {
    
        for (var prop in obj) {
            if (typeof obj[prop] == "object" && obj[prop]) {
                callback(prop);
                traverse(obj[prop]);
            } else {
                callback(prop, obj[prop]);
            }
        }
   
}

traverse(jsonArr);
