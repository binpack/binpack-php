cc -c bin_pack.c -o bin_pack.o
cc -c test-binpack.c -o test-binpack.o
cc -o test-binpack test-binpack.o bin_pack.o
./test-binpack
