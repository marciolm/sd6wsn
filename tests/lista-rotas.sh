curl -g http://[fd00::200:0:0:1] | grep via | cut -f2 -d "]" | cut -f1 -d"/" | sort -u
