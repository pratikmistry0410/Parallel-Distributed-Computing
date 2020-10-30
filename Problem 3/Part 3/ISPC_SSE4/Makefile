compile:
	ispc -O2 --arch=x86-64 --target=sse4-i32x16 square_root.ispc -o objectfiles/square_root_ispc.o -h objectfiles/square_root_ispc.h
	clang++ square_root_main.cpp -Iobjectfiles/ -O2 -m64 -c -o objectfiles/square_root.o
	clang++ -Iobjectfiles/ -O2 -m64 -o square_root_ispc_sse4 objectfiles/square_root.o objectfiles/square_root_ispc.o objectfiles/square_root_ispc_sse4.o -lm -lpthread -lstdc++
