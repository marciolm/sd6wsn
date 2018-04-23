#!/bin/bash
for ((i=1;i<=30;i++));
do 
	./simul20nodes-street-nogui.sh &
	sleep 180
	node ./scripts/sd6wsn-controller-v53.js
	while [ -f "COOJA.testlog" ]; do
		sleep 10 ;
	done
done
