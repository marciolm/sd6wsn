#!/bin/bash

[ "$1" ] || { echo "Uso: $0 <numero total de motes>" ; exit 1 ; }
num_motes=$1
for ((i=1;i<=$num_motes;i++)); do 
   ihex=`echo "ibase=10;obase=16;$i"|bc`
   echo "================================================================"
   ping6 -c 1 -w 2 fd00::200:0:0:$ihex
done
