#!/bin/bash

rm runtime.csv
touch runtime.csv

#Collect Data
for size in 64 128 256 512 1024
do
    for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
    do
        export OMP_NUM_THREADS=$threads
        ./main $size 100 .005
        echo ", ${size}, ${threads}," >> runtime.csv
    done
done