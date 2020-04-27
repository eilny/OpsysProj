#!/bin/sh

echo "compiling:  g++ -Wall -Werror *.cpp -o a.out\n"
g++ -Wall -Werror *.cpp -o a.out

echo "test 5 - alpha .1\n"
./a.out 64 0.001 4096 8 4 0.1 2048 > results/output_5_a01.txt
touch simout.txt
mv simout.txt results/simout05_a01.txt

echo "test 5 - alpha .2\n"
./a.out 64 0.001 4096 8 4 0.2 2048 > results/output_5_a02.txt
touch simout.txt
mv simout.txt results/simout05_a02.txt

echo "test 5 - alpha .3\n"
./a.out 64 0.001 4096 8 4 0.3 248 > results/output_5_a03.txt
touch simout.txt
mv simout.txt results/simout05_a03.txt

echo "test 5 - alpha .4\n"
./a.out 64 0.001 4096 8 4 0.4 2048 > results/output_5_a04.txt
touch simout.txt
mv simout.txt results/simout05_a04.txt

echo "test 5 - alpha .5\n"
./a.out 64 0.001 4096 8 4 0.5 2048 > results/output_5_a05.txt
touch simout.txt
mv simout.txt results/simout05_a05.txt

echo "test 5 - alpha .6\n"
./a.out 64 0.001 4096 8 4 0.6 2048 > results/output_5_a06.txt
touch simout.txt
mv simout.txt results/simout05_a06.txt

echo "test 5 - alpha .7\n"
./a.out 64 0.001 4096 8 4 0.7 2048 > results/output_5_a07.txt
touch simout.txt
mv simout.txt results/simout05_a07.txt

echo "test 5 - alpha .8\n"
./a.out 64 0.001 4096 8 4 0.8 2048 > results/output_5_a08.txt
touch simout.txt
mv simout.txt results/simout05_a08.txt

echo "test 5 - alpha .9\n"
./a.out 64 0.001 4096 8 4 0.9 2048 > results/output_5_a09.txt
touch simout.txt
mv simout.txt results/simout05_a09.txt

echo "test 5 - alpha .01\n"
./a.out 64 0.001 4096 8 4 0.01 2048 > results/output_5_a001.txt
touch simout.txt
mv simout.txt results/simout05_a001.txt

echo "test 5 - alpha .99\n"
./a.out 64 0.001 4096 8 4 0.99 2048 > results/output_5_a099.txt
touch simout.txt
mv simout.txt results/simout05_a099.txt
