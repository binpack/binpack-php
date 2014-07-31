cc -c bin_pack.c -o bin_pack.o
cc test-binpack.o bin_pack.o -o test-binpack
cc -o test-binpack test-binpack.o bin_pack.o
./test-binpack
