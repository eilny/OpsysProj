#!/bin/sh

# test _1 compile (gcc; g++; javac; python -m py_compile project1.py)

#gcc -Wall project.c -lm -D DISPLAY_MAX_T=1000

# test _2
echo "test 2\n"
./a.out 2 0.01 256 1 4 0.5 128 > results/output_2.txt
touch simout.txt
mv simout.txt results/simout02.txt
./a.out 2 0.01 256 1 4 0.5 128 BEGINNING > results/output_2bgn.txt
touch simout.txt
mv simout.txt results/simout02bgn.txt

# test _3
echo "test 3\n"
./a.out 2 0.01 256 2 4 0.5 128 > results/output_3.txt
touch simout.txt
mv simout.txt results/simout03.txt
./a.out 2 0.01 256 2 4 0.5 128 BEGINNING > results/output_3bgn.txt
touch simout.txt
mv simout.txt results/simout03bgn.txt

# test _4
echo "test 4\n"
./a.out 2 0.01 256 16 4 0.5 128 > results/output_4.txt
touch simout.txt
mv simout.txt results/simout04.txt
./a.out 2 0.01 256 16 4 0.5 128 BEGINNING > results/output_4bgn.txt
touch simout.txt
mv simout.txt results/simout04bgn.txt

# test _5
echo "test 5\n"
./a.out 64 0.001 4096 8 4 0.5 2048 > results/output_5.txt
touch simout.txt
mv simout.txt results/simout05.txt
./a.out 64 0.001 4096 8 4 0.5 2048 BEGINNING > results/output_5bgn.txt
touch simout.txt
mv simout.txt results/simout05bgn.txt
