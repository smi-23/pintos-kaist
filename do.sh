cd threads
make clean
make
cd build
source ../../activate
# pintos --gdb -- run alarm-single
pintos -- -q run alarm-multiple