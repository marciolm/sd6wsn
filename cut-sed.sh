grep "DATA" $1 | grep -v "send to :1:" | cut -f1,2,4,6 -d: | head -1300 | tail -1250 | sed -e 's/:/;/g' > rpl-$1.csv
grep "DATA" $1 | grep -v "send to :1:" | cut -f1,2,4,6 -d: | tail -1500 | sed -e 's/:/;/g' > sd6wsn-$1.csv
