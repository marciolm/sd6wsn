#!/bin/bash
num_motes=15
echo "{" > output.json
for ((i=2;i<=$num_motes-1;i++)); do
    ihex=`echo "ibase=10;obase=16;$i"|bc` 	
    ~/libcoap/examples/coap-client -B 10 -m get coap://[fd00::200:0:0:$ihex]:5683/sd6wsn/nbr-etx | grep neigh >> output.json;
    echo "," >> output.json
done
num_motes_hex=`echo "ibase=10;obase=16;$num_motes"|bc`
~/libcoap/examples/coap-client -B 10 -m get coap://[fd00::200:0:0:$num_motes_hex]:5683/sd6wsn/nbr-etx | grep neigh >> output.json
echo "}" >> output.json
