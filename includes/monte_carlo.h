#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

unsigned int setupRandom(unsigned int);
float randomFloat(void);
int randomInt(int);
int randomIntOf(int, int);
void *smalloc (unsigned int);
int sumIntArray(int*, int);
float sumFloatArray(float*, int);
void geomProgression(float*, float, float, int);
void linearProgression(float*, float, float, int);
void createNeighboursMatrix(int**, int);
void populateNeighbours(int, int*, int*, int*, int*);
int neighbourOutOfLattice(int, int, int);
void labelCluster(int*, int*, int**, int, int);
int unionFind(int*, int, int);
bool isValidInteraction(int, int, int, int);
int getInterfacialLengthWith(int*, int**, int, int, int, int);
bool isOnNonPeriodicBorder(int, int, int);

unsigned int setupRandom(unsigned int seed){
  if(seed == 0){
    seed = time(NULL);
  }
	srand(seed);
  return seed;
}

float randomFloat(void){
  return (float)rand()/((float)RAND_MAX);
}
int randomInt(int endExclusive){
  return randomIntOf(0, endExclusive);
}
int randomIntOf(int startInclusive, int endExclusive){
  int size = endExclusive - startInclusive;
  return startInclusive + (rand() % size);
}



/************************************************
		Safe Memory Allocation
			-lpcamors(11/24)
	Triggers an error message if malloc(int) fails  
 ************************************************/
void *smalloc (unsigned int size){
  void *ptr = malloc(size);
  if (ptr == NULL) {
      printf("Error: malloc got NULL!\n");
      exit(EXIT_FAILURE);
  }
  return ptr;
}

void sfree(){

}


/************************************************
		Sum of Integers of an array
			-lpcamors(11/24)  
 ************************************************/
int sumIntArray(int *arr, int size){
  int sum = 0;
  for(int i = 0; i < size; i ++){
    sum += arr[i];
  }
  return sum;
}
/************************************************
		Sum of Floats of an array
			-lpcamors(11/24)  
 ************************************************/
float sumFloatArray(float *arr, int size){
  float sum = 0;
  for(int i = 0; i < size; i ++){
    sum += arr[i];
  }
  return sum;
}

/************************************************
		Geometrical Progression Generator
			-lpcamors(11/24)
	Maps the "*arr" with geometrically distributed
	elements between "start" and "end" (inclusive)  
 ************************************************/
void geomProgression(float *arr, float start, float end, int size){
  int N = size - 1;
  float a = pow(end / start, 1 / (float) N);// 2^{size + 1} = x
  for(int i = 0; i < size; i++){
    arr[i] = start * pow(a, i);
  }
  
}

/************************************************
		Linear Progression Generator
			-lpcamors(11/24)
	Maps the "*arr" with linear distributed
	elements between "start" and "end" (inclusive)  
 ************************************************/
void linearProgression(float *arr, float start, float end, int size){
  int N = size - 1;
  float a = (end - start) / (float) N;
  for(int i = 0; i < size; i++){
    arr[i] = start + (a * (float) i);
  }
}

/************************************************
		Create Neighbours Matrix
			-lpcamors(11/24)
	Maps the "**neighbours" with the neighbours
	of the matrix index. Periodically boundry*   
 ************************************************/
void createNeighboursMatrix(int** neighbours, int N){
  int *up, *down, *right, *left;
  up = smalloc(N * sizeof(int));
  down = smalloc(N * sizeof(int));
  right = smalloc(N * sizeof(int));
  left = smalloc(N * sizeof(int));

  for(int i = 0; i < N; i++){
    neighbours[i] = smalloc(4 * sizeof(int));
  }

  populateNeighbours(N, up, down, right, left);

  for(int j = 0; j < N; j++){
    neighbours[j][0] = right[j];
    neighbours[j][1] = left[j];
    neighbours[j][2] = up[j];
    neighbours[j][3] = down[j];
  }
  free(up);
  free(down);
  free(right);
  free(left);
}

void populateNeighbours(int N, int *up, int *down, int *right, int *left){
  int L = (int) sqrt(N);
  for(int i = 0; i < N; i++){
      if(i % L == 0) { // LADO ESQUERDO
          left[i] = i + L - 1;
          right[i] = i + 1;
      } else if(i % L == L - 1) { // LADO DIREITO
          right[i] = i - L + 1;
          left[i] = i - 1;
      } else { // MEIO
          right[i] = i + 1;
          left[i] = i - 1;
      }
      if(i < L) { // CIMA
          up[i] = i + N - L;
          down[i] = i + L;
      } else if(i >= N - L) { // BAIXO
          down[i] = i - N + L;
          up[i] = i - L;
      } else { // MEIO
          up[i] = i - L;
          down[i] = i + L;
      }
  }
}

bool isValidInteraction(int s_index, int v_index, int N,int b){
	if(b == 0) return true;
	int L = (int) sqrt(N);
	bool bottom_interaction = s_index >= N - L && v_index < L;
	bool up_interaction = s_index < L && v_index >= N - L;
	bool right_interaction = s_index % L == L - 1 && v_index % L == 0;
	bool left_interaction = s_index % L == 0 && v_index % L == L - 1;
	if(b != 1 && (right_interaction || left_interaction)){
		return false;
	}
	if(b != 2 && (up_interaction || bottom_interaction)){
		return false;
	}
	return true;
}
bool isOnNonPeriodicBorder(int s, int N, int B){
	if(B == 0) return false;
	int L = (int) sqrt(N);
	if(B != 2 && (s < L || s >= N - L)) return true;
	if(B != 1 && (s % L == 0 || s % L == L - 1)) return true;
	return false;
}

/************************************************
		Create Clusters Label(H-K)
			-lpcamors(11/24)
	Labels the cluster with same value in "*s",
	it needs the #populateNeighbours() index 
	configuration.
		"b" = "boundry condition"  
			0 -> periodic
			1 -> dobrushin vertical(periodic horizontally)
			2 -> dobrushin horizontal(periodic vertically)
			3 -> square		   
 ************************************************/

void labelCluster(int *label, int *s, int **viz, int N, int b){
	int L = (int) sqrt(N);
    for(int i = 0; i < N; i++){
        label[i] = i;    
    }
    for(int i = 0; i < N; i++){
        if(s[i] == s[viz[i][0]] && (i % L != L - 1 || (b == 0 || b == 1))){//RIGHT
            label[i] = unionFind(label, i, viz[i][0]);
        }
        if(s[i] == s[viz[i][3]] && (i < N - L || (b == 0 || b == 2))){//DOWN
            label[i] = unionFind(label, i, viz[i][3]);
        }

    }
    return;
}

int unionFind(int *lab, int i1_0, int i2_0){
    int i1 = i1_0, i2 = i2_0, I;
    while(i1 != lab[i1]){
        i1 = lab[i1];    
    }
    while(i2 != lab[i2]){
        i2 = lab[i2];
    }
    I = i1;
    if(i1 < i2) I = i2;
    for(int i = 0; i <= I; i++){
    	if(lab[i] == i1 || lab[i] == i2){
    		lab[i] = I;
    	}
    }
    return I;
}
/************************************************
		Interfacial Length of Cluster with ...
			-lpcamors(12/24)
	Measure the length of the cluster(l1) with the
	other cluster(l2). Uses the H-K algorithm. Sen
	sitive to boundry conditions. 
 ************************************************/

int getInterfacialLengthWith(int* lab, int** neig, int l1, int l2, int N, int B){
	int li, lj, length = 0;
	for(int i = 0; i < N; i++){
		li = lab[i];
		if(li != l1) continue;
		for(int j = 0; j < 4; j++){
			if(!isValidInteraction(i, neig[i][j], N, B)) continue;
			lj = lab[neig[i][j]];
			if(lj != l2) continue;
			length += 1;
		}
	}
	return length;
}

int neighbourOutOfLattice(int s, int n, int N){
  int L = (int) sqrt(N);
  if( (s < L && n == 2) || (s >= N - L && n == 3) || (s % L == 0 && n == 1) || (s % L == L - 1 && n == 0) ){
    return 1;
  }
  return 0;
}

