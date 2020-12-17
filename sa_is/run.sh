#!/bin/bash

g++ SA-IS.cpp -o SA-IS
echo -e "\e[32mSA_IS.cpp compiled. \e[m"
g++ doubling_sa.cpp -p doubling_sa
echo -e "\e[32mdoubling_sa.cpp compiled. \e[m"
echo -e "\e[32mSA-IS executing. \e[m"
./SA-IS
echo -e "\e[32mSA-IS finished. \e[m"
echo -e "\e[32mdoubling_sa executing. \e[m"
./doubling_sa
echo -e "\e[32mAll finished. \e[m"