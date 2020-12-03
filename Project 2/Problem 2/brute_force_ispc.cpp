// The main recursive method to print all possible strings of length "length"
uniform size_t MD5_DIGEST_LENGTH=16;

// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

 
static inline void md5(unsigned int8 *initial_msg, size_t initial_len, unsigned int64 * dig_low, unsigned int64 * dig_up) {
 
    unsigned int64 h0, h1, h2, h3;

    // Message (to prepare)
    unsigned int8 *msg = NULL;

    // unsigned int8 initial_msg[] = {48};
 
    // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
 
    // r specifies the per-round shift amounts
 
    unsigned int32 r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    // Use binary integer part of the sines of integers (in radians) as constants// Initialize variables:
    unsigned int32 k[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
 
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;
 
    // Pre-processing: adding a single 1 bit
    //append "1" bit to message    
    /* Notice: the input bytes are considered as bits strings,
       where the first bit is the most significant bit of the byte.[37] */
 
    // Pre-processing: padding with zeros
    //append "0" bit until message length in bit ≡ 448 (mod 512)
    //append length mod (2 pow 64) to message
 
    int new_len;
    for(new_len = initial_len*8 + 1; new_len%512!=448; new_len++);
    new_len /= 8;
 
    msg = (unsigned int8 *)(new int8[new_len + 64]);
    memset(msg,0,new_len + 64);
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 128; // write the "1" bit
    unsigned int32 bits_len = 8*initial_len; // note, we append the len
    memcpy(msg + new_len, &bits_len, 4);           // in bits at the end of the buffer
 
    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    int offset;
    for(offset=0; offset<new_len; offset += (512/8)) {
 
        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        unsigned int32 *w = (unsigned int32 *) (msg + offset);
 
        // Initialize hash value for this chunk:
        unsigned int32 a = h0;
        unsigned int32 b = h1;
        unsigned int32 c = h2;
        unsigned int32 d = h3;
 
        // Main loop:
       unsigned int32 i;
        for(i = 0; i<64; i++) {      

 
            unsigned int32 f, g;
 
             if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3*i + 5) % 16;          
            } else {
                f = c ^ (b | (~d));
                g = (7*i) % 16;
            }
            
            unsigned int32 temp = d;
            d = c;
            c = b;
            // print("rotateLeft(%x + %x + %x + %x, %d)\n\n", a, f, k[i], w[g], r[i]);
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }
 
        // Adding this chunk's hash to result
        h0 = (unsigned int32)h0 + a;
        h1 = (unsigned int32)h1 + b;
        h2 = (unsigned int32)h2 + c;
        h3 = (unsigned int32)h3 + d;
 
    }
 

    delete(msg);
    h0 &= 0x00000000FFFFFFFF;
    h0 |= (h1 << 32) & 0xFFFFFFFF00000000; 
    h2 &= 0x00000000FFFFFFFF;
    h2 |= (h3 << 32) & 0xFFFFFFFF00000000; 
    memcpy(dig_low,&h0,sizeof(int64));
    memcpy(dig_up,&h2,sizeof(int64));
}

export void compare_hashes(uniform const int8 phrases[], uniform const unsigned int64 digest[], uniform unsigned int8 output[], uniform size_t length, uniform int count){
    uniform unsigned int64 dig_up = digest[1];
    uniform unsigned int64 dig_low = digest[0];
    foreach(i = 0 ... count){
        unsigned int64 hash_c_low = 0;
        unsigned int64 hash_c_up = 0;
        md5((unsigned int8 *)&phrases[0] + i * length, length, (unsigned int64 *)&hash_c_low,(unsigned int64 *)&hash_c_up);
        if((hash_c_low == dig_low) && (hash_c_up == dig_up)){
        memcpy(output,(unsigned int8 *)&phrases[0] + i * length,length);
        }
    }
}

task void compare_hashes_task(uniform const int8 phrases[],
    uniform const unsigned int64 digest[], uniform unsigned int8 output[], uniform size_t length, uniform unsigned int count, uniform int task_size){

    uniform int start = taskIndex * task_size;
    uniform int end = min(task_size * (taskIndex + 1), (const uniform unsigned int)count);
    if(count - end < task_size){
        end += count - end;
    }
    uniform unsigned int64 dig_up = digest[1];
    uniform unsigned int64 dig_low = digest[0];
    foreach(i = start ... end){
        unsigned int64 hash_c_low = 0;
        unsigned int64 hash_c_up = 0;
        
        md5((unsigned int8 *)&phrases[0] + i * length, length, (unsigned int64 *)&hash_c_low,(unsigned int64 *)&hash_c_up);
        if((hash_c_low == dig_low) && (hash_c_up == dig_up)){
            memcpy(output,(unsigned int8 *)&phrases[0] + i * length,length);
        }
    }
}

export void compare_hashes_task_launcher(uniform const int8 phrases[],
	uniform const unsigned int64 digest[], uniform unsigned int8 output[], uniform size_t length, uniform int count, uniform int tasks){
	uniform int task_size = count / tasks;
    launch [tasks] compare_hashes_task(phrases, digest, output, length, count, task_size);
}