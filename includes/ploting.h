#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void startLatticeGif(int);
void setColorPallete(char**, int);
void printMatrix(int*, int);
void printLabelAt(char*, char*, float, float);

void startLatticeGif(int L){
	printf("set terminal gif animate delay 20 size 800,800\n");
	printf("set output 'animate.gif'\n");
	printf("set size square \n");
	printf("unset key\n");
	printf("set xrange [-0.5:%.2f]\n", L - 0.5);
	printf("set yrange [%.2f:-0.5]\n", L - 0.5);
}
void setColorPallete(char **colorArr, int size){
	printf("set palette defined (0 \"%s\"", colorArr[0]);
	for(int i = 1; i < size; i++){
		printf(", %d \"%s\"", i, colorArr[i]);
	}
	printf(")\n");
}
void printMatrix(int *s, int L){
	printf("plot '-' matrix with image\n");
	for(int i = 0; i < L; i++){
		for(int j = 0; j < L; j++){
			printf("%d ", s[j + (i * L)]);
		}
		printf("\n");
	}
	printf("e\n");
	fflush(stdout);
}
void printLabelAt(char *valueLabel, char *value, float x, float y){
	printf("set label 1 '%s = %s' at screen %.2f, %.2f center\n", valueLabel, value, x, y);
}
