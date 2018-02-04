hex=`echo "ibase=10;obase=16;$1"|bc`
~/libcoap/examples/coap-client -B 5 -m get coap://[fd00::200:0:0:$hex]:5683/sd6wsn/$2
