#!/bin/bash

set -x;
make clean; make;
rm data.csv;

for p in 1 4 8 12 16 32 64 
do
  mpirun -np $p ./powermethod $(echo "z=sqrt($p*10000000); z - z%$p" | bc) 100
done

for p in 1 4 8 12 16 32 64
do
  mpirun -np $p ./powermethod 10176 1000
done