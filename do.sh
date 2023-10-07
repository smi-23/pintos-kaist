# cd threads
# make clean
# make
# cd build
# source ../../activate
# # pintos --gdb -- run alarm-single
# # pintos -- -q run alarm-multiple
# # pintos -- -q run alarm-priority
# # pintos -- -q run priority-change
# # pintos -- -q run priority-preempt
# # pintos -- -q run priority-fifo
# pintos -- -q run priority-donate-sema
cd userprog
make clean
make -j $(nproc --all)
cd build
pintos-mkdisk filesys.dsk 10
# pintos --fs-disk filesys.dsk -p tests/userprog/args-single:args-single -- -q -f run 'args-single onearg'
pintos --fs-disk filesys.dsk -p tests/userprog/bad-read:bad-read -- -f run 'bad-read'
# pintos --fs-disk filesys.dsk -p tests/userprog/args-single:args-single --gdb -- -f run 'args-single onearg'