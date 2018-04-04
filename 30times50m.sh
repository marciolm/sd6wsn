#!/bin/bash
for ((i=1;i<=1;i++));
do 
	./simul20nodes-street-50m-nogui.sh &
	sleep 240
	node ./scripts/sd6wsn-controller-v53.js
	while [ -f "COOJA.testlog" ]; do
		sleep 10 ;
	done
done
