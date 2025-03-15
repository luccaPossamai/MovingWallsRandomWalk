#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fhelper.h>
#include <monte_carlo.h>

#define L		128
#define FORCE_SEED  0
#define DETA        1e-4
#define TMAX        1e6
#define TTICS       100
#define SCALE       0 // 0 -> LINEAR
                      // 1 -> LOG
void simulate(void);
void setup(void);
void mergeTemporaryValues(void);
int onTimeSimulate(float);

void takeMeasures(float);
void onParticleUpdate(int);


FILE *f;
unsigned int seed;
int xA, xB, xM, tempXA, tempXB, tempXM;
float etaA, etaB, timeT;


int main() {
    seed = setupRandom(FORCE_SEED ? 123456789 : 0);

    char name[50];
    sprintf(name, "data/data_W_L%d", L);
    f = safeSeedOpen(name, ".dat", &seed, 0);
    fprintf(f, "#t, xA, xB, xM, W\n");
    simulate();
    fclose(f);
    
	return 0;
}

void simulate(void){
    setup();
    float *t_arr = smalloc(TTICS * sizeof(float));
    if(SCALE){
        geomProgression(t_arr, 0, TMAX, TTICS);
    } else {
        linearProgression(t_arr, 0, TMAX, TTICS);
    }
    float nextTime;
    for(int i = 0; i < TTICS; i++){
        nextTime = t_arr[i];
        while(onTimeSimulate(nextTime)){}
    }
    
    

}
void setup(){
    xA = (int)(-L / 2);
    xB = (int)( L / 2);
    xM = 0;
    etaA = 0.0;
    etaB = 0.0;
    timeT = 0.0;
    tempXA = xA;
    tempXB = xB;
    tempXM = xM;
}

void mergeTemporaryValues(){
    xA = tempXA;
    xB = tempXB;
    xM = tempXM;
}

int onTimeSimulate(float timeTarget) {
    
    if(timeT >= timeTarget){
        takeMeasures(timeTarget);
        return 0;
    }
    mergeTemporaryValues();
	timeT += 1.0 / 3.0;
	onParticleUpdate(randomIntOf(0, 3));
    return 1;
}
void takeMeasures(float timeTarget){
    fprintf(f, "%.2f %d %d %d %d\n", timeTarget, xA, xB, xM, xB - xA);
}
//0 = A, 1 = M, 2 = B;
void onParticleUpdate(int particle){
    
    switch(particle){
        case 1:
            tempXM += randomIntOf(-1, 2);
            if(tempXM == tempXA) tempXA--;
            if(tempXM == tempXB) tempXB++;
            break;
        case 0:
            etaA += DETA;
            if(etaA >= 1.0){
                etaA = 0;
                tempXA++;
                if(tempXA == tempXM){
                    tempXM++;
                }
            }
            break;
        case 2:
            etaB += DETA;
            if(etaB >= 1.0){
                etaB = 0;
                tempXB--;
                if(tempXB == tempXM){
                    tempXM--;
                }
            }
            break;
        
    }
    
}








