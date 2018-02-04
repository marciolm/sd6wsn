while [ 1 ]; do
	cat ./motes.txt | while read MOTE; do	
         	echo "{\"date\"="`date +%Y%m%d%H%M%S`"}" >> $MOTE.txt;
		~/libcoap/examples/coap-client -B 5 -m get coap://[$MOTE]:5683/sd6wsn/nbr-etx | grep nei >> $MOTE.txt;
	done
sleep 120;
done
