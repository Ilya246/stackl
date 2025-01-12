mkdir -p out
git submodule init
git submodule update
g++ stackl.cpp -o out/stackl -Og -g
