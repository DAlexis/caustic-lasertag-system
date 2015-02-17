#!/bin/bash

echo "Usage: fix-drills.sh <filename>.kicad_pcb"

echo "0.762mm => 0.800mm"
sed -i s/"drill 0.762"/"drill 0.800"/g $1
echo "0.9652mm => 1.000mm"
sed -i s/"drill 0.9652"/"drill 1.000"/g $1
echo "1.016mm => 1.000mm"
sed -i s/"drill 1.016"/"drill 1.000"/g $1
echo "3.048mm => 3.000mm"
sed -i s/"drill 3.048"/"drill 3.000"/g $1
