// This is start of the header guard.  ADD_H can be any unique name.  By convention, we use the name of the header file.
#ifndef _BRUTE_FORCE_H
#define _BRUTE_FORCE_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <pthread.h>
#include <chrono>  // for high_resolution_clock
#include <stdint.h>
#include <openssl/md5.h>
#include <sstream>
#include "objs/brute_force_ispc.h"

using namespace std; 
using Clock = chrono::steady_clock;
using chrono::time_point;
using chrono::duration_cast;
using chrono::milliseconds;
// using namespace literals::chrono_literals;

void gen_strs(const char*,string,const int, const int);

void gen_arr(char *);

void md5_to_str(unsigned char*);

void str_to_md5(const char*, unsigned char  *);

int load_hashes(char *, const size_t, const char*);

void compare_hashes_serial(const uint8_t * ,const uint64_t * ,uint8_t * ,size_t, int);

void md5(const uint8_t * , size_t, uint64_t *, uint64_t *); 

#endif
