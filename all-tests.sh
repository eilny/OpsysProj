#!/bin/sh

# test _1 compile (gcc; g++; javac; python -m py_compile project1.py)

#gcc -Wall project.c -lm -D DISPLAY_MAX_T=1000

# test _2
./a.out 2 0.01 256 1 4 0.5 128 > results/output_2.txt
touch simout.txt
mv simout.txt results/simout02.txt

# test _3
./a.out 2 0.01 256 2 4 0.5 128 > results/output_3.txt
touch simout.txt
mv simout.txt results/simout03.txt

# test _4
./a.out 2 0.01 256 16 4 0.5 128 > results/output_4.txt
touch simout.txt
mv simout.txt results/simout04.txt

# test _5
./a.out 64 0.001 4096 8 4 0.5 2048 > results/output_5.txt
touch simout.txt
mv simout.txt results/simout05.txt
