#!/bin/bash
dtseed=$(date '+%H%M%S');
cat simul-sd6wsn-20motes-street-150m.csc | sed -e "s/<randomseed>123456/<randomseed>$dtseed/g" > $dtseed.csc
java -mx512m -jar /home1/user/contiki/tools/cooja/dist/cooja.jar -nogui="/home1/user/contiki/examples/sd6wsn/$dtseed.csc" -contiki="/home1/user/contiki"
dt=$(date '+%d%m%Y-%H%M%S');
mv ./COOJA.testlog COOJA.testlog.$dt.txt
rm $dtseed.csc
