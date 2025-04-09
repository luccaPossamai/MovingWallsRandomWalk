#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fhelper.h>
#include <monte_carlo.h>

#define FORCE_SEED  0

//  Temporal Evolution   #
#define DETA        1e-4
#define TMAX        1e7
#define TMIN        1
//  Non temporal Evolution
#define TTICS       1
#define LOOPS       1e5

// For dEta Evolution
#define DETAMAX     1e-2
#define DETAMIN     1e-3

// Evolution Parameter Scale
#define SCALE       1 // 0 -> LINEAR
                      // 1 -> LOG
#define COLLISION_RESET 0
// Mode
#define MODE        0 // 0 -> temporal
                      // 1 -> correlation time
void simulate(void);
void setup(void);
void setupFile(char*, int);
void mergeTemporaryValues(void);
int onTimeSimulate(float);
void writeSpecifications(FILE*);
void dEtaEvolution(void);
void dEtaEvolutionDist(void);
float timeForDEta(float, int);
void takeMeasures(float);
void onParticleUpdate(int);
double wStatForDeta(float);



FILE *f, *fCompl;
unsigned int seed;
int xA, xB, xM, tempXA, tempXB, tempXM, W0 = 2;
float dEta = DETA;
double timeT;
float etaA, etaB, lastMeasure = 0;
int *distXm, *distXWalls, spectedWStat, boundDist;


int main() {
    seed = setupRandom(FORCE_SEED ? 123456789 : 0);

    if(MODE == 0){
        setupFile("data_rwmw_MEAN_w", 1);
        dEtaEvolution();
        fclose(fCompl);    
    } else if(MODE == 2){
        setupFile("data_rwmw_W", 0);
        simulate();
        fclose(f);
    } else if(MODE == 3){
        dEtaEvolutionDist();
    }
    
    
	return 0;
}

void setupFile(char* name, int isCompl){
    if(isCompl){
        fCompl = safeSeedOpen(name, ".dat", &seed, 0);
        writeSpecifications(fCompl);
        return;
    }
    f = safeSeedOpen(name, ".dat", &seed, 0);
    writeSpecifications(f);
}

void simulate(void){
    setup();
    float *t_arr = smalloc(TTICS * sizeof(float));
    if(SCALE){
        geomProgression(t_arr, (int)TMIN, (int)TMAX, TTICS);
    } else {
        linearProgression(t_arr, (int)TMIN, (int)TMAX, TTICS);
    }
    float nextTime;
    for(int i = 0; i < TTICS; i++){
        nextTime = t_arr[i];
        while(onTimeSimulate(nextTime)){}
    }
    free(t_arr);
}

void dEtaEvolution(void){
    float *dEta_arr = smalloc(TTICS * sizeof(float));
    if(SCALE){
        geomProgression(dEta_arr, DETAMIN, DETAMAX, TTICS);
    } else {
        linearProgression(dEta_arr, DETAMIN, DETAMAX, TTICS);
    }
    for(int i = 0; i < TTICS; i++){
        dEta = dEta_arr[i];
        char name[50] = "";
        sprintf(name, "data_rwmw_w_dEtae%.5f", dEta);
        setupFile(name, 0);
        lastMeasure = 0;
        setup();
        float t_stat = timeForDEta(dEta, 1);
        while(onTimeSimulate(t_stat)){}
        
        for(int i = 0; i < LOOPS; i++){
            t_stat += timeForDEta(dEta, 0);
            while(onTimeSimulate(t_stat)){}
        }
        fprintf(fCompl, "%.5f %.2f\n", dEta, lastMeasure / (float) LOOPS);
        fclose(f);
    
    }
    free(dEta_arr);

}


void dEtaEvolutionDist(void){
    float *dEta_arr = smalloc(TTICS * sizeof(float));
    if(SCALE){
        geomProgression(dEta_arr, DETAMIN, DETAMAX, TTICS);
    } else {
        linearProgression(dEta_arr, DETAMIN, DETAMAX, TTICS);
    }
    for(int i = 0; i < TTICS; i++){
        dEta = dEta_arr[i];
        spectedWStat = wStatForDeta(dEta);
        boundDist = spectedWStat;
        distXm = smalloc((boundDist) * sizeof(int));
        distXWalls = smalloc((boundDist) * sizeof(int));
        for(int i = 0; i < boundDist ; i++){
            distXm[i] = 0;
            distXWalls[i] = 0;
        }
        char name[50] = "";
        sprintf(name, "data_rwmw_dist_dEtae%.5f", dEta);
        setupFile(name, 0);
        setup();
        float t_stat = timeForDEta(dEta, 1);
        while(onTimeSimulate(t_stat)){}
        
        for(int i = 0; i < LOOPS; i++){
            t_stat += timeForDEta(dEta, 0) + 1;
            while(onTimeSimulate(t_stat)){}
        }
        for(int i = 0; i < boundDist; i++){
            double pos = (i / (double) boundDist) * spectedWStat;
            fprintf(f, "%.4f %.5f\n", pos, distXm[i] / ((float)(LOOPS)));
        }
        fclose(f);
        
        free(distXm); free(distXWalls);
    }
    free(dEta_arr);

}

void setup(){
    xA = (int)(-W0 / 2);
    xB = (int)( W0 / 2);
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
    if(timeT >= 8.99e6){
        //printf("%d %d %d\n", xA, xM, xB);
    }
    if(timeT >= timeTarget){
        takeMeasures(timeTarget);
        return 0;
    }
    mergeTemporaryValues();
	timeT += 1.0 / 3.0;
	onParticleUpdate(randomIntOf(0, 3));
    return 1;
}

void writeSpecifications(FILE *file){
    int isCompl = file == fCompl;
    fprintf(file, "# Inertial Walls on Random Walk\n");
	fprintf(file, "# Data generated by: L. Possamai\n");
	fprintf(file, "# Seed: %d\n", seed);
	if(SCALE == 0){
		fprintf(file, "# Linear Tics: %d\n", TTICS);
	} else {
		fprintf(file, "# Log Tics: %d\n", TTICS);
	}
	switch(MODE){
	    case 0:
	        fprintf(file, "# dEta: %.5f\n", dEta);
	        fprintf(file, "# t, xA, xB, xM, W\n");
	        break;
	    case 1:
	        fprintf(file, "# t, W(0)W(t), W, W2\n");
	        break;    
	    case 2:
	    
	        fprintf(file, "# dEta: [%.5f, %.5f]\n", (float) DETAMIN, (float)DETAMAX);
	        if(!isCompl){
	            fprintf(file, "# t dEta W_stat\n"); 
	        } else {
	            fprintf(file, "# Measures: %d\n", (int) LOOPS);
	            fprintf(file, "# dEta <W_stat>\n");
	        }
	        break;
	    case 3:
	        if(!isCompl){
	            fprintf(file, "# dEta: %.5f\n", dEta);
	            fprintf(file, "# Wstat: %d\n", spectedWStat);
	            fprintf(file, "# x, p(xm = x), p(xW = x)\n");
	        }
	        break;
	        
	        
	}
    
}
float timeForDEta(float dEta, int forStat){
    return forStat ? 1.0e1 / (dEta * dEta) : 1.0e1 / dEta;
}
double wStatForDeta(float dEta){
    return (2.0 / dEta) + 1;
}

void takeMeasures(float timeTarget){
    if(MODE == 0){
        
        fprintf(f, "%.2f %d %d %d %d\n", timeTarget, xA, xB, xM, xB - xA);
    } else if(MODE == 1){
        if(timeTarget == TMIN){
            lastMeasure = xB - xA;
        }

        fprintf(f, "%.2f %d %d %d\n", timeTarget, (int)lastMeasure * (xA - xB), xB - xA, (xB - xA) * (xB - xA));
        
    } else if(MODE == 2){
        lastMeasure += xB - xA;
        fprintf(f, "%.2f %.5f %d\n", timeTarget, dEta, (xB - xA));
    } else if(MODE == 3){
        int w = xB - xA;
        double pA = abs(xM - xA) / (double)w;
        double pB = abs(xM - xB) / (double)w;
        distXm[(int)(pA * boundDist)] += 1;
        distXm[(int)(pB * boundDist)] += 1;
        //double f = boundDist / (double)w;// pos / w = (0, 1/2)
        //int index = (int)(2 * f * pos);
        //distXm[index] += 1;
    }
    
}
//0 = A, 1 = M, 2 = B;
void onParticleUpdate(int particle){
    
    switch(particle){
        case 1:
            // decides on which direction the inner particle will walk
            int p = (randomFloat() < .5) ? 1 : -1;
            tempXM += p;
            // if it hits some of the outer particles these one gets pushed 
            if(tempXM == tempXA) {
                tempXA--;
                etaA = 0;
                if(COLLISION_RESET){
                    tempXM -= p;
                }
            }
            if(tempXM == tempXB) {
                tempXB++;
                etaB = 0;
                if(COLLISION_RESET){
                    tempXM -= p;
                }
            }
            break;
        case 0:
            etaA += dEta;
            if(etaA >= 1.0){
                etaA = 0;
                // outer particles walks
                tempXA++;
                if(tempXA == tempXM){
                    // if outer particles hits inner particle
                    // it pushes the inner one
                    tempXM++;
                    if(tempXM == tempXB){
                        // if the inner particle hits the other outer particle
                        // return to the intial state
                        tempXM--;
                        tempXA--;
                    }
                }
            }
            break;
        case 2:
            etaB += dEta;
            if(etaB >= 1.0){
                etaB = 0;
                tempXB--;
                
                if(tempXB == tempXM){
                    tempXM--;
                    if(tempXM == tempXA) {
                        tempXM++;
                        tempXB++;
                    }
                }
            }
            break;
        
    }
    
}








