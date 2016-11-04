#!/bin/bash

rm *.png

for f in *.gv
do
	dot -Tpng $f -o$f.png
done
