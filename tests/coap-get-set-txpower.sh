num_motes=6
for ((i=2;i<=$num_motes;i++)); do 
	echo "================================================================"
   	echo "Potencia atual do mote cooja$i  " 
	~/libcoap/examples/coap-client -B 5 -m get coap://[fd00::200:0:0:$i]:5683/sd6wsn/txpower | tail -1
done
~/libcoap/examples/coap-client -B 5 -m put coap://[fd00::200:0:0:2]:5683/sd6wsn/txpower?index=19
~/libcoap/examples/coap-client -B 5 -m put coap://[fd00::200:0:0:3]:5683/sd6wsn/txpower?index=19
~/libcoap/examples/coap-client -B 5 -m put coap://[fd00::200:0:0:4]:5683/sd6wsn/txpower?index=19
~/libcoap/examples/coap-client -B 5 -m put coap://[fd00::200:0:0:5]:5683/sd6wsn/txpower?index=19
~/libcoap/examples/coap-client -B 5 -m put coap://[fd00::200:0:0:6]:5683/sd6wsn/txpower?index=44
for ((i=2;i<=$num_motes;i++)); do 
	echo "================================================================"
   	echo "Potencia nova do mote cooja$i  " 
	~/libcoap/examples/coap-client -B 5 -m get coap://[fd00::200:0:0:$i]:5683/sd6wsn/txpower | tail -1
done

#	3    0x03 -> -18 dBm
#	44   0x2C -> -7 dBm
#	136  0x88 -> -4 dBm
#	129  0x81 -> -2 dBm
#	50   0x32 -> 0 dBm
#	19   0x13 -> 1 dBm
#	171  0xAB -> 2 dBm
#	242  0xF2 -> 3 dBm
#	247  0xF7 -> 5 dBm
#  {  7, 0xFF },
#  {  5, 0xED },
#  {  3, 0xD5 },
#  {  1, 0xC5 },
#  {  0, 0xB6 },
#  { -1, 0xB0 },
#  { -3, 0xA1 },
#  { -5, 0x91 },
#  { -7, 0x88 },
#  { -9, 0x72 },
#  {-11, 0x62 },
#  {-13, 0x58 },
# {-15, 0x42 },
#  {-24, 0x00 },


