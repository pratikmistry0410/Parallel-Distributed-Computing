compile:
	ispc -O2 --arch=x86-64 --target=sse2-i32x4,sse4-i32x8,avx1-i32x16,avx2-i32x16,avx512knl-i32x16,avx512skx-i32x16 square_root.ispc -o objectfiles/square_root_ispc.o -h objectfiles/square_root_ispc.h
	clang++ square_root_main.cpp -Iobjectfiles/ -O2 -m64 -c -o objectfiles/square_root.o
	clang++ -Iobjectfiles/ -O2 -m64 -o square_root objectfiles/square.o objectfiles/square_root_ispc.o objectfiles/square_root_ispc_sse2.o objectfiles/square_ispc_sse4.o objectfiles/square_root_ispc_avx.o objectfiles/square_root_ispc_avx2.o objectfiles/square_root_ispc_avx512knl.o objectfiles/square_root_ispc_avx512skx.o -lm -lpthread -lstdc++
