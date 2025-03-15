#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define BPB		8

void set_bit(unsigned char *array, int index, int value) {
    if (value == 1) {
    	array[index / BPB] |= (1 << (index % BPB)); // Define o bit como 1
    } else {
		array[index / BPB] &= ~(1 << (index % BPB)); // Define o bit como 0
    }
        
}

int get_bit(unsigned char *array, int index) {
    return (array[index / BPB] >> (index % BPB)) & 1; // Retorna o valor do bit
}	

void compactArray(unsigned char *array, int *s, int N){
	for(int i = 0; i < N; i++){
		set_bit(array, i, s[i]);
	}
}

void decompactArray(unsigned char *array, int *s, int N){
	for(int i = 0; i < N; i++){
		s[i] = get_bit(array, i);
	}
}


