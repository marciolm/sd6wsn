#!/bin/bash
echo -n 'insert?index=2&ipv6dst=”2001:0DB8:10::20”&nhmacaddr=”00124B000144F978”&txpwr=”3”' | coap put coap://[fd00::200:0:0:2]:5683/sd6wsn/flow-mod
