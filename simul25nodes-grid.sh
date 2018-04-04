#!/bin/bash
simul=simul-sd6wsn-25motes.csc
make TARGET=cooja $simul &
sleep 240
node ./scripts/sd6wsn-25nodes-app-v2.js >> ./COOJA.testlog


