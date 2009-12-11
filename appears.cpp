#define DDEBUG 0

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include "ddebug.h"

#include <cstdio>
#include <cmath>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <bitset>

/* here we allow overriding on the g++/cl command line */
#ifndef APPEARS_SEQSIZE
#define APPEARS_SEQSIZE 15L /* sequence string size */
#endif
/* 4^APPEARS_SEQSIZE */
const unsigned int SP_TB_NUM = 490000000;
const unsigned int CMP_TB_NUM = 490000000;

const unsigned int masks[4]={0xfff,0xfff*2,0xfff*3,0xfff*4};

const size_t TABLESIZE = 1L << (2 * APPEARS_SEQSIZE);
/* We make this naming convention that a "token" is a nucleotide */
const int BITS_PER_TOKEN = 2;

unsigned int *Table_SP =new unsigned int [SP_TB_NUM];
unsigned int *Table_CMP =new unsigned int [CMP_TB_NUM];

enum {
    G = 0, A, T, C
};



/* forward declarations */
static bool parse_seq_file (const char* fname);
static void search_missing_combinations ();
static void index_to_tokens (unsigned long index, char* buf);

int
main (int argc, char* argv[]) {
    const char* fname_b;
    const char* fname_s;
    if (argc < 2) {
        fprintf(stderr, "ERROR: No input file specified.\n");
        return 1;
    }
    fname_b = argv[1];
    fprintf(stderr, "INFO: Reading file %s...\n", fname_b);

    if ( ! parse_seq_file(fname_b) ) {
        return 1;
    }


    fprintf(stderr,
            "INFO: Searching missing combinations of sequences...\n");
    search_missing_combinations();
for(int i=0;i<4;i++){
printf("%x\n",masks[i]);
}    
return 0;
}

/* parse_seq_file: read in the sequence stream and set
 * the global BitTable_b variable accordingly.
 *这里输入加入一个枚举变量，区别不同表。
 *
 * */
bool
parse_seq_file (const char* fname) {
    FILE* infile;
    char c;
    unsigned long lineno = 1L;
    unsigned long num = 0;
    unsigned long index;
    unsigned long tokens_seen = 0L;

    if (NULL == (infile = fopen(fname, "r"))) {
        fprintf(stderr,
                "ERROR: Failed to open input file %s for reading: "
                "%s\n",
                fname, strerror(errno));
        return false;
    }

    for (;;) {
        bool found_token = false;

        c = getc(infile);
        if (EOF == c) {
            if (ferror(infile)) {
                fprintf(stderr, "ERROR: Failed to read from file %s:"
                        "%s\n",
                        fname, strerror(errno));
                return false;
            }
            break;
        }

        switch (c) {
            case '\n':
                lineno++;
                break;
            case 'G':
                index <<= BITS_PER_TOKEN;
                index |= G;
                found_token = true;
                break;
            case 'A':
                index <<= BITS_PER_TOKEN;
                index |= A;
                found_token = true;
                break;
            case 'T':
                index <<= BITS_PER_TOKEN;
                index |= T;
                found_token = true;
                break;
            case 'C':
                index <<= BITS_PER_TOKEN;
                index |= C;
                found_token = true;
                break;
            default:
                found_token = false;
                break;
        }
        if (found_token) {
            if (tokens_seen < APPEARS_SEQSIZE) {
                tokens_seen++;
            }
            if (tokens_seen >= APPEARS_SEQSIZE) {
                /* register this subsequence combination */
                Table_SP[num]=index;
		num++;
		tokens_seen=0L;
            }
        }
    }
    fclose(infile);
    return true;
}

/* index_to_tokens: decode the binary index numerals to G/A/T/C
 * token sequences */
void
index_to_tokens (unsigned int index, char* buf) {
    for (unsigned int i = 0; i < APPEARS_SEQSIZE; i++) {
        int encoded_token =
            (index >> (i * BITS_PER_TOKEN)) & 3L;
        switch (encoded_token) {
            case G:
                buf[APPEARS_SEQSIZE - i - 1] = 'G';
                break;
            case A:
                buf[APPEARS_SEQSIZE - i - 1] = 'A';
                break;
            case T:
                buf[APPEARS_SEQSIZE - i - 1] = 'T';
                break;
            case C:
                buf[APPEARS_SEQSIZE - i - 1] = 'C';
                break;
            default:
                break;
        }
    }
    buf[APPEARS_SEQSIZE] = '\0';

}

/* search_missing_combinations: traverse the BitTable_b
 * to find 0 bits and print out the corresponding
 * character sequences */
void
search_missing_combinations () {
char buf[APPEARS_SEQSIZE+1 ];
unsigned int cmp;
long i,j;
int k;
unsigned int sp[4];


for (i=0;i<49;i++){
sp[0]=(Table_SP[i]<<1) & masks[0];
sp[1]=(Table_SP[i]<<2) & masks[1];
sp[2]=(Table_SP[i]<<3) & masks[2];
sp[3]=(Table_SP[i]<<4) & masks[3];
	for (j=0;j<1;j++){
		if(sp[0]==Table_CMP[j]);{
			index_to_tokens (Table_CMP[i], buf);
			printf("%s:%ld\n",buf,i);
			contiune;
		}
		if(sp[1]==Table_CMP[j]);{
			index_to_tokens (Table_CMP[i], buf);
			printf("%s:%ld\n",buf,i);
			contiune;
		}
		if(sp[2]==Table_CMP[j]);{
			index_to_tokens (Table_CMP[i], buf);
			printf("%s:%ld\n",buf,i);
			contiune;
		}
		if(sp[3]==Table_CMP[j]);{
			index_to_tokens (Table_CMP[i], buf);
			printf("%s:%ld\n",buf,i);
			contiune;
		}
	}
}








