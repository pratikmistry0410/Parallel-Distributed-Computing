square_root_avx: square_root_avx.cpp
	# gcc -mavx --target=avx2-i32x16 -o square_root_avx square_root_avx.cpp -march=native -O3 -lstdc++ -lm --std=c99
	gcc -mavx --target=sse4-i32x16 -o square_root_avx square_root_avx.cpp -march=native -O3 -lstdc++ -lm --std=c99