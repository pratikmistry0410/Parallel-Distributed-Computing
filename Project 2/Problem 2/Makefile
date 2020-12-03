CC = g++ -std=c++11
CFLAGS = -Wall -g -I${CONC_DIR}/
LFLAGS = -lm -lpthread -lstdc++
CONC_DIR = objs

all: brute_force

brute_force: brute_force.cpp $(CONC_DIR) tasksys.o brute_force_ispc.o brute_force.o generate_words
	$(CC) ${CFLAGS} ${LFLAGS} -o brute_forcer ${CONC_DIR}/tasksys.o ${CONC_DIR}/brute_force.o ${CONC_DIR}/brute_force_ispc.o

brute_force.o: brute_force.cpp brute_force.h
	$(CC) $(CFLAGS) -c brute_force.cpp -o ${CONC_DIR}/brute_force.o

brute_force_ispc.o: brute_force_ispc.cpp
	ispc -g ${OPTFLAG} -o ${CONC_DIR}/brute_force_ispc.o brute_force_ispc.cpp -h ${CONC_DIR}/brute_force_ispc.h --target=avx2-i32x8

tasksys.o: tasksys.cpp
	$(CC) $(CFLAGS) tasksys.cpp -O2 -m64 -c -o ${CONC_DIR}/tasksys.o

$(CONC_DIR):
	mkdir -p $@ 

generate_words:
ifeq (,$(wildcard ./table1.txt))
	python gen.py
endif

clean:
	rm -r ${CONC_DIR}
	rm brute_forcer
	rm table*.txt
