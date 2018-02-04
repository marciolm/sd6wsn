while [ 1 ]; do
        cat ./motes.txt | while read MOTE; do
        ~/libcoap/examples/coap-client  -B 2 -m post coap://[$MOTE]/actuators/toggle  1>/dev/null &
        done
sleep 3;
done
