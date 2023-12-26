
#include <stdint.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>

#define REPEAT 1000000
#define SIZE 2097152

inline void clflush (volatile void *p) {
	// printf("%p address of where start point\n", (void *) p);
	// printf("%p address of where end point\n", (void *) p_end);
	// printf("%c inside before flush\n", *(int *)p);
	// char *p_end = (char *)p + SIZE-1;
	// asm volatile("IC IALLUIS" : : : "memory"); -> illegal hardware instruction
	// dcache_clean_inval_poc(p, p_end);
	//__builtin___clear_cache((char *)p, (char *) p_end);
	// printing can mess with the debugging, as printing can take up ticks
	
	asm volatile("dc civac, %0" :: "r" ((void *)p) : "memory");
}
 
inline uint64_t rdtsc() {
	uint64_t val = 0;
    asm volatile("mrs %0, cntvct_el0" : "=r" (val));                    
    return val;
}

char lineBuffer[SIZE];
long int rep;

inline void memtest() {
	uint64_t start, end, clock;
	char* lineBuffer = (char*) malloc(SIZE); 
	char* lineBufferCopy = (char*) malloc(SIZE); 
	for (int i = 0; i < SIZE; i ++){
		lineBuffer[i] = '1'; 
	}

	// printf("%p buff address first\n", &lineBuffer[0]);
	// printf("%p buff address last\n", &lineBuffer[SIZE-1]);
	// printf("%c at buff first\n", lineBuffer[0]);
	// printf("%c at buff last\n", lineBuffer[SIZE-1]);

	clock = 0;
	// printf("%llu ticks start\n", clock); 

	for (rep = 0; rep < REPEAT; rep++){
		start = rdtsc();
		asm volatile("dsb sy": : : "memory");
        asm volatile("isb sy": : : "memory");
		memcpy(lineBufferCopy, lineBuffer, SIZE); 
		end = rdtsc();
		clflush(lineBuffer);
		clflush(lineBufferCopy);
		clock = clock + (end - start);
		// printf("%llu ticks to copy %dB\n", (end-start), SIZE);
	}
	printf("%llu average ticks \n", clock/REPEAT); 
	printf("%llu total ticks \n", clock); 
}


int main(int ac, char **av) {
	printf("==============================\n"); 
	memtest();
	return 0;
}