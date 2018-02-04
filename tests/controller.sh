#!/bin/bash

[ "$1" ] || { echo "Uso: $0 <numero total de motes>" ; exit 1 ; }
num_motes=$1
coap_client=~/libcoap/examples/coap-client
 
pkill -9 lt-coap-client

#Registra os motes para observe do etx
for ((i=2;i<=$num_motes-1;i++)); do
 ihex=`echo "ibase=10;obase=16;$i"|bc` 
 echo "mote=$ihex"	
 $coap_client -B 3600 -s 3600 -o $i.txt -m get coap://[fd00::200:0:0:$ihex]:5683/sd6wsn/info-get/nbr-etx &
 sleep 0.5
done

#Envia n fluxos (de acordo com o numero de rotas RPL) para os motes periodicamente, simulando alteracoes
#while [ 1 ]; do
  for ((i=2;i<=$num_motes-1;i++)); do
    ihex=`echo "ibase=10;obase=16;$i"|bc` 
    routes=`$coap_client -B 5 -m get coap://[fd00::200:0:0:$ihex]:5683/sd6wsn/info-get/routes | grep -v CON`
    let routes++
    echo "mote=$ihex"
    echo $routes
      for ((j=1;j<=$routes;j++)); do
         $coap_client -B 5 -m put coap://[fd00::200:0:0:$ihex]:5683/sd6wsn/flow-mod/action=insert?i=2,id=2001:0DB8:10::20,n=F978 &
      done
  done
#  sleep 1200;
#done


# $coap_client -B 5 -m put coap://[fd00::200:0:0:4]:5683/sd6wsn/flow-mod/insert?index=2&ipv6dst=”2001:0DB8:10::20”&nhmacaddr=”00124B000144F978”&txpwr=”3”



