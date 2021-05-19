#!/bin/bash

rm -rf test
mkdir test
g++ testcase_gen.cpp -o testcase_gen
./testcase_gen
g++ bit_vector.cpp -o bit_vector
echo -e "\e[32mexecuting bit_vector.cpp \e[m"
g++ simple.cpp -o simple
echo -e "\e[32mexecuting simple.cpp \e[m"
/usr/bin/time -f "MAX MEMORY SIZE %M KB" ./bit_vector
/usr/bin/time -f "MAX MEMORY SIZE %M KB" ./simple
echo -e "\e[32mall finished. \e[m"