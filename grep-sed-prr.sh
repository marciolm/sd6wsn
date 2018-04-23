tail -n4 *.txt | grep PRR | cut -f3 -d " " | sed -e 's/\./,/g'
