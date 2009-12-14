#define DDEBUG 0

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include "ddebug.h"
#include "stdint.h"

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
const uint32_t SP_TB_NUM = 49; // 15bp table rows
const uint32_t CMP_TB_NUM = 1; // 12bp table rows

const int BITS_PER_TOKEN = 2;
const uint32_t masks[4]={0xffffff,0xffffff*BITS_PER_TOKEN*2,0xffffff*BITS_PER_TOKEN*3,0xffffff*BITS_PER_TOKEN*4};

const size_t TABLESIZE = 1L << (2 * APPEARS_SEQSIZE);
/* We make this naming convention that a "token" is a nucleotide */
uint32_t* Table_SP =new uint32_t [SP_TB_NUM];
uint32_t* Table_CMP =new uint32_t [CMP_TB_NUM];

enum {
    G = 0, A, T, C
};

enum {
	TB_SP,TB_CMP 
};


/* forward declarations */
static bool parse_seq_file (const char* fname,uint32_t* Table,uint32_t SEQSIZE);
static void search_missing_combinations ();
static void index_to_tokens (uint32_t index, char* buf); 
static void d_print(uint32_t index, char* buf);
int
main (int argc, char* argv[]) {
    const char* fname_sp;
    const char* fname_cmp;
    if (argc < 2) {
        fprintf(stderr, "ERROR: No input file specified.\n");
        return 1;
    }
    fname_sp = argv[1];
    fprintf(stderr, "INFO: Reading file %s...\n", fname_sp);

    if ( ! parse_seq_file(fname_sp,Table_SP,15) ) {
        return 1;
    }

    fname_cmp = argv[2];
    fprintf(stderr, "INFO: Reading file %s...\n", fname_cmp);

    if ( ! parse_seq_file(fname_cmp,Table_CMP,12) ) {
        return 1;
    }

    fprintf(stderr,
            "INFO: Searching missing combinations of sequences...\n");
    search_missing_combinations();
/*
 * debug
 *
for(int i=0;i<4;i++){
printf("%x\n",masks[i]);

}   
 */

delete [] Table_SP;
delete [] Table_CMP;
return 0;
}

/* parse_seq_file: read in the sequence stream and set
 * the global BitTable_b variable accordingly.
 *这里输入加入一个枚举变量，区别不同表。
 *
 * */
bool
parse_seq_file (const char* fname,uint32_t* Table,uint32_t SEQSIZE) {
    FILE* infile;
    char c;
    uint32_t lineno = 1L;
    int num = 0;
    uint32_t index;
    uint32_t tokens_seen = 0L;

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
            if (tokens_seen < SEQSIZE) {
                tokens_seen++;
            }
            if (tokens_seen >= SEQSIZE) {
                /* register this subsequence combination */
                Table[num]=index;
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
index_to_tokens (uint32_t index, char* buf) {
    for (uint32_t i = 0; i < APPEARS_SEQSIZE; i++) {
        uint32_t encoded_token =
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
uint32_t cmp;
uint32_t i,j;
uint32_t sp[4];
int match;

for (i=0;i<SP_TB_NUM ;i++){
	sp[0]=(Table_SP[i]>>BITS_PER_TOKEN*0) & masks[0];
	sp[1]=(Table_SP[i]>>BITS_PER_TOKEN*1) & masks[0];
//			d_print (sp[1], buf);
	sp[2]=(Table_SP[i]>>BITS_PER_TOKEN*2) & masks[0];
//			d_print (sp[2], buf);
	sp[3]=(Table_SP[i]>>BITS_PER_TOKEN*3) & masks[0];
//			d_print (sp[3], buf);
	match=0;
		for (j=0;j<CMP_TB_NUM;j++){
			cmp=Table_CMP[j]&masks[0];
//				d_print (cmp, buf);
			
			if(sp[0]==cmp){
				match=1;
				continue;
			}
			if(sp[1]==cmp){
				match=1;
				continue;
			}
			if(sp[2]==cmp){
				match=1;
				continue;
			}
			if(sp[3]==cmp){
				match=1;
				continue;
			}
		}
		if(!match){
			index_to_tokens (Table_SP[i], buf);
			printf("%s:%i\n",buf,i);
		}
}

}
void d_print(uint32_t index, char* buf) {
			index_to_tokens (index, buf);
			printf("%s\n",buf);
}
