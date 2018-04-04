#!/bin/bash
pkill -9 tail
dt=$(date '+%d%m%Y-%H%M%S')
simul=simul-sd6wsn-25motes.csc
dtseed=$(date '+%H%M%S');
cat $simul | sed -e "s/<randomseed>123456/<randomseed>$dtseed/g" > $dtseed.csc
rm /COOJA.testlog
java -mx512m -jar /home1/user/contiki/tools/cooja/dist/cooja.jar -nogui="/home1/user/contiki/examples/sd6wsn/$dtseed.csc" -contiki="/home1/user/contiki" &
sleep 30
tail -f ./COOJA.testlog >> ./COOJA.testlog.$dt.txt &
sleep 240
node ./scripts/sd6wsn-25nodes-app-v2.js >> ./COOJA.testlog.$dt.txt
rm $dtseed.csc

