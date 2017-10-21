/*Cache Simulator
 *Spring 2016 - 1.0
 *Fall 2017 - 2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <strings.h>
#include <limits.h>
#include <assert.h>

typedef unsigned long long int address; //store address

//Cache structs
typedef struct{
	int s; //2^s sets
	int E; //E = E lines
	int b; //B = B^b bytes
} cache_input;

typedef struct {
	int S; //2^s
	int B; //2^b
}cache_help;

typedef struct{
	char* t;
	char* s;
	char* b;
} address_struct; //currently unused

int vflag =0; //verbose flag


//Summarize cache simulation stats at end
void printSummary(int hits, int misses, int evictions){
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

int main(int argc, char **argv) {
    //set cache up reading in input
	cache_input *newc = malloc(sizeof(cache_input));
    char* filename = (char*)malloc(sizeof(char)*20);
    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i],"-v")==0)
            vflag++;
        if (strcmp(argv[i],"-s")==0)
            newc->s = atoi(argv[i+1]);
        if (strcmp(argv[i],"-E")==0)
            newc->E = atoi(argv[i+1]);
        if (strcmp(argv[i],"-b")==0)
            newc->b = atoi(argv[i+1]);
        if (strcmp(argv[i],"-t")==0 && i < argc - 1)
            filename = argv[i+1];
    }
    printf("v = %d, s = %d, E = %d, b = %d\n", vflag, newc->s, newc->E, newc->b);
    
	if (filename == NULL) {
		printf("No file selected\n");
		//exit(1);
	}

	//set cache help
	cache_help  *newchelp = malloc(sizeof(cache_help));
	newchelp->S = 1 << newc->s; 
	newchelp->B = 1 << newc->b;
	printf("s = %d, S = %d, b = %d, B = %d\n", newc->s, newchelp->S, newc->b, newchelp->B);

	typedef struct{
		int valid;
		address tag;
		int timehelp;
	}line; //for one line (E)

	typedef struct{
		line *lines;
	}set; //for one set (S), all lines

	typedef struct{
		set *sets;
	}cache; //for one cache (C), all sets 

	cache C;
	C.sets = malloc(sizeof(set)*newchelp->S);

	for (int j=0; j<newchelp->S; j ++) {
		C.sets[j].lines = malloc(sizeof(line)*newc->E);
	}

	char instruction;
	int size;

	int hits = 0; 
	int misses = 0;
	int evictions = 0;

	int LRU = 0;        //value for Least Replacement Used replacement policy
	int empty = -1;     //index of empty space
	int HIT = 0;        //boolean representing if there was a hit or not
	int EVICTION = 0;   //boolean representing if there was an eviction or not
	int evicthelp = 0;  //helper for what to evict;
	address A;          //a given address

	FILE *f = fopen(filename, "r");
	if (access(filename,F_OK)==-1) {
        fprintf(stderr,"error (main): file \"%s\" does not exist\n",filename);
        //exit(1);
 	}

  	while (fscanf(f, "%c %llx,%d", &instruction, &A, &size) > 0) {
  		if (instruction == 'L' || instruction == 'S' || instruction == 'M') {
			printf("%c, %llx, %d \n", instruction, A, size);
			address TAG = A >> (newc->s + newc->b);
			int tbits = 64 - (newc->s + newc->b);
			long long setnum = (A << tbits) >> (tbits + newc->b);
			set SET = C.sets[setnum];
			int LEAST = INT_MAX;
		
			for (int i=0; i< newc->E; i++) {
				if (SET.lines[i].valid==1) {
					if (SET.lines[i].tag == TAG) { //check for hit
						hits += 1;
						HIT = 1;
						SET.lines[i].timehelp = LRU;
						LRU++;
					} else if (SET.lines[i].timehelp < LEAST) {
						LEAST = SET.lines[i].timehelp;
						evicthelp = i;
					}
				} else if (empty == -1){
					empty = i;
				}
			}

			if (HIT != 1) {     //for misses
				misses += 1;
				if (empty > -1) {
					SET.lines[empty].valid = 1;
					SET.lines[empty].tag = TAG;
					SET.lines[empty].timehelp = LRU;
					LRU += 1;
				} else if (empty < 0) {
					EVICTION = 1;
					SET.lines[evicthelp].tag = TAG;
					SET.lines[evicthelp].timehelp = LRU;
					LRU += 1;
					evictions += 1;
				}
			}

			if (instruction == 'M')
				hits += 1;
			if (vflag == 1)
				printf("%c, %llx, %d \n", instruction, A, size );
			
            if (HIT == 1){
				printf("HIT!\n");
			} else {
				printf("MISS!\n");
			}
            
			if(EVICTION == 1){
				printf("EVICTION!\n");
			}
            
			empty = -1;
			HIT = 0;
			EVICTION = 0;
		}
	}
	fclose(f);

	for(int k=0; k<newchelp->S; k++){
		free(C.sets[k].lines);
	}
	free(C.sets);
	free(newc);
	free(newchelp);
    free(filename);
   	printSummary(hits, misses, evictions);
   	return 0;
}
