while [ 1 ]; do
	cat ./motes.txt | while read MOTE; do	
         	echo "{\"date\"="`date +%Y%m%d%H%M%S`"}" >> $MOTE-rssi.txt;
		~/libcoap/examples/coap-client -B 10 -m get coap://[$MOTE]:5683/sd6wsn/rssi | grep rssi >> $MOTE-rssi.txt;
	done
sleep 60;
done
