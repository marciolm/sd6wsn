#!/bin/bash
simul=simul-sd6wsn-25motes.csc
java -mx512m -jar /home1/user/contiki/tools/cooja/dist/cooja.jar -nogui="/home1/user/contiki/examples/sd6wsn/$simul" -contiki="/home1/user/contiki"
dt=$(date '+%d%m%Y-%H%M%S');
mv ./COOJA.testlog COOJA.testlog.$dt.$simul.txt

