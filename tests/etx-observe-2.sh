num_motes=6
pingok=""
for ((i=2;i<=$num_motes;i++)); do 	
	while [  "$pingok" = "" ]; do
		sleep 1;
		pingok=`ping6 -c1 -W 1 fd00::200:0:0:$i -q | grep " 0%" 2>/dev/null`;
	done
	pingok="" 
	~/libcoap/examples/coap-client -B 3600 -s 3600 -o $i.txt -m get coap://[fd00::200:0:0:$i]:5683/sd6wsn/nbr-etx &

done

