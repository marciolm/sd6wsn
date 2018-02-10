#!/bin/bash
grep DATA $1 | sed -e 's/:/;/g' > $1.csv

