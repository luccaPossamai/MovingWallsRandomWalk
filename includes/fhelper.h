#include <stdio.h>
#include <stdlib.h>

FILE* safeOpen(char*, char*);
FILE* safeSeedOpen(char*, char*, unsigned int*, int);
int existFile(char*);
int getIntFromUser(char*, int, int);

FILE* safeOpen(char *name, char *extension){
	FILE *f;
	char name0[50];
	int i = 0;
	do {
		sprintf(name0, "%s_%d%s",name, i, extension);
		i += 1;
	} while (existFile(name0) == 1);
	f = fopen(name0, "w");
	return f;
}

int existFile(char *name){
	FILE *f;
	int i = 0;
	f = fopen(name, "r");
	if(f != NULL) {
		i = 1;
		fclose(f);
	}
	return i;
}

FILE* safeSeedOpen(char *name, char *extension, unsigned int *seed, int forceSeed){
	FILE *f;
	char name0[150];
	if(*seed % 2 == 0) *seed += 1;
	do {
		sprintf(name0, "%s_S%d%s",name, *seed, extension);
		*seed += 2;
		
	} while (existFile(name0) == 1 && forceSeed == 0);
	f = fopen(name0, "w");
	return f;
}

int getIntFromUser(char *message, int min, int max){
	char input[50], *output, message2[100];
	int value, i;
	sprintf(message2, "%s[%d, %d]", message, min, max);
	do{
		printf("\n%s: ", message2);
		i = scanf("%50s", input);
		value = (int)strtol(input, &output, 10);
	} while(i <= 0 || *output != '\0' || value < min || value > max);
	return value;
}


