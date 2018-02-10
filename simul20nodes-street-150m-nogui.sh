#!/bin/bash
java -mx512m -jar /home1/user/contiki/tools/cooja/dist/cooja.jar -nogui="/home1/user/contiki/examples/sd6wsn/simul-sd6wsn-20motes-street-150m.csc" -contiki="/home1/user/contiki"
dt=$(date '+%d%m%Y-%H%M%S');
cp ./COOJA.testlog COOJA.testlog.$dt.txt

