#!/bin/bash

# rm -rf test
# mkdir test
# g++ testcase_gen.cpp -o testcase_gen
# echo -e "\e[32mtestcase generating. \e[m"
# ./testcase_gen

# g++ -std=c++17 wavlet_tree.cpp -o wavlet_tree
# echo -e "\e[32mwavlet_tree.cpp compiled. \e[m"
# ./wavlet_tree
# echo -e "\e[32mwavlet_tree.cpp executing. \e[m"

# g++ wavlet_matrix.cpp -o wavlet_matrix
# echo -e "\e[32mwavlet_matrix.cpp compiled. \e[m"
# ./wavlet_matrix
# echo -e "\e[32mwavlet_matrix.cpp executing. \e[m"

echo -e "\e[32mwavlet_tree.cpp executing. \e[m"
/usr/bin/time -f "MAX MEMORY SIZE %M KB" ./wavlet_tree
echo -e "\e[32mwavlet_matrix.cpp executing. \e[m"
/usr/bin/time -f "MAX MEMORY SIZE %M KB" ./wavlet_matrix