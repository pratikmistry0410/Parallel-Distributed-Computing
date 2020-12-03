#include "brute_force.h"

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

unsigned char digest[MD5_DIGEST_LENGTH];

int main(int argc, char * argv[]){
    if(argc < 3){
        cout << "Correct usage of program: brute_force_password_cracker <md5 hash> <number of tasks> \n";
        exit(1);
    } else if(strlen(argv[1]) != 32){
        cout << "MD5 not correct length\n";
        exit(1);
    }
    // const char md5[]= "0fdc3cbc9a749efcaf083440a794fae4";                // Testing Hash
    char * md5 = argv[1];
    //These are the number of elements to be sorted taken as an input with the program
    int tasks  = atoi(argv[2]);                     // Total number of tasks

    int length = 8;                             // Max Length of Password
    unsigned char  * digest2 = (unsigned char * )calloc(1,MD5_DIGEST_LENGTH);
    uint8_t * output = new uint8_t[length]();
    unsigned int count = pow(62,length);
    char * phrases;

    str_to_md5(md5, digest2);
    md5_to_str(digest2);
                           
    time_point<Clock> start, end;
    long elapsed = 0;
    for(int i = 1; i <= length;i++){
        count = pow(62,i);
        phrases = (char*) calloc(count * i, 2*sizeof(char ));
        char filename[10];
        printf("[AVX] Starting passwords of size %d\n",i);
        sprintf(filename, "table%d.txt",i);
        load_hashes(phrases,i,filename);
        start = Clock::now();
        ispc::compare_hashes((const int8_t *)phrases,(const uint64_t *)digest2,output,i, count);
        end = Clock::now();
        free(phrases);
        elapsed += duration_cast<milliseconds>(end - start).count();
        if(strlen((char*)output) > 0){
            printf("[AVX]Password: %s\n",output);
            break;
        }
    }

    printf("[AVX] MD5 hash comparison of %d "
           "entries took %ld ms\n\n", count, elapsed);


    memset(output,0,length);
    elapsed = 0;
    for(int i = 1; i <= length;i++){
        count = pow(62,i);
        phrases = (char*) calloc(count * i, sizeof(char ));
        if(phrases == NULL){
            printf("Failed to allocate space\n");
            exit(1);
        }
        char filename[10];
        printf("[TASKS] Starting passwords of size %d\n",i);
        sprintf(filename, "table%d.txt",i);
        load_hashes(phrases,i,filename);
        start = Clock::now();
        ispc::compare_hashes_task_launcher((const int8_t *)phrases,(const uint64_t *)digest2,output,i, count, tasks);
        end = Clock::now();
        free(phrases);
        elapsed += duration_cast<milliseconds>(end - start).count();
        if(strlen((char*)output) > 0){
            printf("[TASKS] Password: %s\n",output);
            break;
        }
    }

    printf("[TASKS] MD5 hash comparison of %d "
           "entries took %ld ms with %d tasks\n\n", count, elapsed, tasks);



    memset(output,0,length);
    elapsed = 0;
    for(int i = 1; i <= length;i++){
        count = pow(62,i);
        phrases = (char*) calloc(count * i, 2*sizeof(char ));
        char filename[10];
        printf("[SERIAL] Starting passwords of size %d\n",i);
        sprintf(filename, "table%d.txt",i);
        load_hashes(phrases,i,filename);
        start = Clock::now();
        compare_hashes_serial((const uint8_t *)phrases,(const uint64_t *)digest2,output,i, count);
        end = Clock::now();
        free(phrases);
        elapsed += duration_cast<milliseconds>(end - start).count();
        if(strlen((char*)output) > 0){
            printf("[SERIAL] Password: %s\n",output);
            break;
        }
    }

    printf("[SERIAL] MD5 hash comparison of %d "
           "entries took %ld ms\n\n", count, elapsed);
    return 0;

}

void compare_hashes_serial(const uint8_t * phrases,const uint64_t * digest,uint8_t * output,size_t length, int count){
    uint64_t dig_up = digest[1];
    uint64_t dig_low = digest[0];
    for(int i = 0; i < count;i++){
        uint64_t hash_c_low = 0;
        uint64_t hash_c_up = 0;
        md5(&phrases[0] + i * length, length, &hash_c_low,&hash_c_up);
    if((hash_c_low == dig_low) && (hash_c_up == dig_up)){
        memcpy(output,(uint8_t *)&phrases[0] + i * length,length);
    }
    }
}

int load_hashes(char * phrases, const size_t phrase_size, const char* filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        fprintf(stderr, "File %s could not be opened!\n", filename);
        fclose(file);
        return 0;
    }
    char * phrase_line = (char*)calloc(phrase_size, sizeof(char));
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int counter = 0;
while ((read = getline(&line, &len, file)) != -1) {
        memcpy(&phrases[0] + counter * phrase_size, line, phrase_size);
        counter++;
    }
    cout << "Loaded all the hashes... " << counter << endl;
    fclose(file);
    free(phrase_line);
    return 1;
}


void gen_arr(char arr[]){
    for(char i = 0;i < 95;i++){
      arr[(int)i] = i + 32;
    }
}

void str_to_md5(const char* str, unsigned char  * digest2){
    int n = strlen(str);
    char * endptr;
    for(int i = 0; i < n;i= i + 2){
        char lett[3];
        lett[2] = 0;
        strncpy(lett,str + i,2);
        long num = strtol(lett, &endptr, 16);
        digest2[i / 2] = num;
    }
}

void md5_to_str(unsigned char* md) {
    int i;
    for(i=0; i <MD5_DIGEST_LENGTH; i++) {
            printf("%02x",md[i]);
    }
    printf("\n");
}


void md5(const uint8_t *initial_msg, size_t initial_len, uint64_t * dig_low, uint64_t * dig_up) {

    uint64_t h0, h1, h2, h3;

 
    // Message (to prepare)
    uint8_t *msg = NULL;
 
    // Note: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating
 
    // r specifies the per-round shift amounts
 
    uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    // Use binary integer part of the sines of integers (in radians) as constants// Initialize variables:
    uint32_t k[] = {
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
 
    msg = (uint8_t*)calloc(new_len + 64, 1); // also appends "0" bits 
                                   // (we alloc also 64 extra bytes...)
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 128; // write the "1" bit
 
    uint32_t bits_len = 8*initial_len; // note, we append the len
    memcpy(msg + new_len, &bits_len, 4);           // in bits at the end of the buffer
 
    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    int offset;
    for(offset=0; offset<new_len; offset += (512/8)) {
 
        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        uint32_t *w = (uint32_t *) (msg + offset);
 
        // Initialize hash value for this chunk:
        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
 
        // Main loop:
        uint32_t i;
        for(i = 0; i<64; i++) {

 
            uint32_t f, g;
 
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

            uint32_t temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }
 
        // Add this chunk's hash to result so far:
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    free(msg);
    h0 &= 0x00000000FFFFFFFF;
    h0 |= (h1 << 32) & 0xFFFFFFFF00000000; 
    h2 &= 0x00000000FFFFFFFF;
    h2 |= (h3 << 32) & 0xFFFFFFFF00000000; 
    memcpy(dig_low,&h0,sizeof(uint64_t));
    memcpy(dig_up,&h2,sizeof(uint64_t));
}