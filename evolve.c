#include "evaluate.c"

// SOME GLOBAL VARIABLES
double best_of_run_fit;
double best_of_run_genome[GENOME_SIZE];

void initialize_genome(double *g, int size) {
    int i;
    for (i=0; i<size; i++) {
        g[i] = randf();
    }
}

void mutate (double *genome) {
    int i;
    for (i=0; i < GENOME_SIZE; i++) {
	genome[i] += gaussrand() * 0.01;
	while (genome[i] < 0 || genome[i] > 1) {
	    if (genome[i] < 0) {
		genome[i] = -genome[i];
	    }
	    else {
		genome[i] = 2 - genome[i];
	    }
	}
    }
}

void copy_genome(double *to, double *from) {
    int i;
    for (i=0; i < GENOME_SIZE; i++) {
	to[i] = from[i];
    }
}

void print_genome(double *ind) {
    int i;
    for (i=0; i < GENOME_SIZE; i++) {
	printf("%f ", ind[i]);
    }
    printf("\n");
}



void evaluate_population(double **population) {
    int i;
    int best_ind = 0;
    double best_fit = 0.0;
    double sum_fit = 0.0;

    for (i=0; i < POPULATION_SIZE; i++) {
	double ind_fit = evaluate(population[i], 0);
	sum_fit += ind_fit;
	if (ind_fit > best_fit) {
	    best_fit = ind_fit;
	    best_ind = i;
	}
    }
    printf("BEST: %f  MEAN: %f\n", best_fit, sum_fit / POPULATION_SIZE);

    if (best_fit > best_of_run_fit) {
	best_of_run_fit = best_fit;
	copy_genome(best_of_run_genome, population[best_ind]);
    }
}


void evolve(double **population) {
    int i, gen, print;
    best_of_run_fit = 0;
    double fit1 = 0;
    double fit2 = 0;
    for (gen = 0; gen < GENERATIONS; gen++) {
	int index1 = rand() % POPULATION_SIZE;
	i = (rand() % DEMES_SIZE) - (DEMES_SIZE / 2);
	int temp = (index1 + i) % POPULATION_SIZE;
	if (temp < 0) {
	    temp += POPULATION_SIZE;
	}
	int index2 = temp;
	double *ind1 = population[index1];
	double *ind2 = population[index2];

	double ind1_fit = evaluate(ind1, 0);
	double ind2_fit = evaluate(ind2, 0);
	double win_fit;
	double *win;
	double *lose;
	if (ind1_fit >= ind2_fit) {
	    win = ind1;
	    lose = ind2;
	    win_fit = ind1_fit;
	}
	else {
	    win = ind2;
	    lose = ind1;
	    win_fit = ind2_fit;
	}
	copy_genome(lose, win);
	mutate(lose);
	printf("%5d: ", gen);
	evaluate_population(population);

    }
    printf("BEST OF RUN:\n");
    printf("GENOME: ");
    print_genome(best_of_run_genome);
    printf("FITNESS: %f\n",  best_of_run_fit);
    evaluate(best_of_run_genome, 1);
}

void clean_up() {
}

main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr, "usage: %s seed\n", argv[0]);
	exit(1);
    }
    else {
	srand(atoi(argv[1]));
    }

    int i;


// STILL ROBOT
/*
    double test_ind[] = {.0, .0, .0, //biases
                         .5, .5, .5, //sensor weights
			 .0, .0, .0, //time constants
			 .5, .5, .5, .5, .5, .5, .5, .5, .5}; //weights

    evaluate(test_ind, 1);
    exit(1);
*/

    double *(population[POPULATION_SIZE]);  // array of pointers to type double
    for (i=0; i < POPULATION_SIZE; i++) {
	population[i] = (double*)malloc(sizeof(double) * GENOME_SIZE);
	initialize_genome(population[i], GENOME_SIZE);
    }
    evolve(population);
    clean_up();
}
