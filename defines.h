#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//filenames
#define TRACE_FILENAME   "trace_out.dat"

//output
#define PRINT_FOOD_POSITION      0
#define PRINT_ROBOT_INFO         0
#define PRINT_ROBOT_COORDS       0
#define PRINT_FITNESS            0
#define PRINT_ANN_STATE          0

//evolution
#define POPULATION_SIZE 50 //paper
#define GENERATIONS 100
#define MUTATION_VARIANCE 0.01 //paper
#define DEMES_SIZE 10 //paper
#define NUM_WALKING_TRIALS 2
#define NUM_CHEMOTAXIS_TRIALS 15

//walking task
#define NUM_WALKING_STEPS 220 //paper

#define NUM_NODES 3
#define GENOME_SIZE ((NUM_NODES * NUM_NODES) + (3 * NUM_NODES))

//chemotaxis task
#define NUM_CHEMOTAXIS_STEPS 1000
//#define NUM_CHEMOTAXIS_STEPS 100 //paper
#define CHASSIS_RADIUS 1 // can't find what this is supposed to be !!!!
//#define NUM_CHEMOTAXIS_FOOD_REPS 3 //paper
#define NUM_CHEMOTAXIS_FOOD_REPS 1
#define LAMBDA -0.0138 //paper
#define MIN_FOOD_DISTANCE 10 //paper
#define MAX_FOOD_DISTANCE 15 //paper

//integraiton
#define TIME_STEP 0.1 //paper

//#define INFINITY 999999999
#define PI 3.1415926535


typedef struct {

} NODE;


typedef struct {
    double *bias;              //theta
    double *time_constants;    //tau
    double *activation;        //y
    double **weights;
    double *sensor_weights;
    
} ANN;


//from knuth, as described at http://c-faq.com/lib/gaussian.html
double gaussrand() {
    static double V1, V2, S;
    static int phase = 0;
    double X;
    if(phase == 0) {
	do {
	    double U1 = (double)rand() / RAND_MAX;
	    double U2 = (double)rand() / RAND_MAX;

	    V1 = 2 * U1 - 1;
	    V2 = 2 * U2 - 1;
	    S = V1 * V1 + V2 * V2;
	} while(S >= 1 || S == 0);

	X = V1 * sqrt(-2 * log(S) / S);
    }
    else {
	X = V2 * sqrt(-2 * log(S) / S);
    }
    phase = 1 - phase;
    return X;
}

double randf() {
    return (double)rand() / (double)RAND_MAX ;
}


