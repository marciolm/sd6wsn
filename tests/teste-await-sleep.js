async function init(){
   console.log(1)
   await sleep(5000)
   console.log(2)
      await sleep(5000)
         console.log(3)
}
function sleep(ms){
    return new Promise(resolve=>{
        setTimeout(resolve,ms)
    })
}
init()
