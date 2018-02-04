grep "<id>" $1 | cut -f2 -d">" | cut -f1 -d"<" > posid.tmp
grep -A 2 "<x>" $1 | grep "<x>" | cut -f2 -d">" | cut -f1 -d"<" > posx.tmp
grep -A 2 "<x>" $1 | grep "<y>" | cut -f2 -d">" | cut -f1 -d"<" > posy.tmp
grep -A 2 "<x>" $1 | grep "<z>" | cut -f2 -d">" | cut -f1 -d"<" > posz.tmp
paste -d "," posid.tmp posx.tmp posy.tmp posz.tmp > pos-motes$1.txt
rm pos*.tmp
