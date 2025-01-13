mkdir -p out
git submodule init
git submodule update
g++ stackl.cpp -o out/stackl -O3 -flto=auto -march=native -mtune=native -std=c++20
