#include "defines.h"

double sigma (double x) {
    return (1 / (1 + exp(-x)));
}


void initialize_ann(ANN *ann) {
    int i;
    ann->y_dot =          (double*)malloc(sizeof(double) * NUM_NODES);
    ann->bias =           (double*)malloc(sizeof(double) * NUM_NODES);
    ann->time_constants = (double*)malloc(sizeof(double) * NUM_NODES);
    ann->activation =     (double*)malloc(sizeof(double) * NUM_NODES);
    ann->sensor_weights = (double*)malloc(sizeof(double) * NUM_NODES);
    ann->weights =        (double**)malloc(sizeof(double*) * NUM_NODES);
    for(i = 0; i < NUM_NODES; i++) {
	ann->weights[i] = (double*)malloc(sizeof(double) * NUM_NODES);
    }
}

// FOR DEBUGGING
void print_ann (ANN *ann) {
    int i;
    fprintf(stderr, "ANN: ");
    for (i=0; i < NUM_NODES; i++) {
	fprintf(stderr, "%d: %f  ", i, ann->activation[i]);
    }
    fprintf(stderr, "\n");
}

void decode_genome(double *ind, ANN *ann) {
    int i, j, gene;
    gene = 0;
    initialize_ann(ann);
    for (i = 0; i < NUM_NODES; i++) {
	ann->y_dot[i] = 0;
    }
    for (i = 0; i < NUM_NODES; i++) {
	ann->activation[i] = 0;
    }
    for (i = 0; i < NUM_NODES; i++) {
	ann->bias[i] = (ind[gene++] * 20) - 10; //map to range [-10, 10]
    }
    for (i = 0; i < NUM_NODES; i++) {
	ann->sensor_weights[i] = (ind[gene++] * 20) - 10; //range [-10, 10]
    }
    for (i = 0; i < NUM_NODES; i++) {
	ann->time_constants[i] = (ind[gene++] * 19) + 1; //map to range [1, 20]
    }
    for (i = 0; i < NUM_NODES; i++) {
	for (j = 0; j < NUM_NODES; j++) {
	    ann->weights[i][j] = (ind[gene++] * 20) - 10; //range [-10, 10]
	}
    }
    if (GENOME_SIZE != gene) {
	fprintf(stderr, "numgenes: %d  index: %d\n", GENOME_SIZE, gene);
	exit(i);
    }
}


double update_ann (ANN *ann, double sensor) {
    int i, j;
    double sum;

    double newActivation[NUM_NODES];
    for (i=0; i < NUM_NODES; i++) {
	newActivation[i] = ann->activation[i] + (TIME_STEP * ann->y_dot[i]);
	sum = 0;
	for (j=0; j < NUM_NODES; j++) {
	    sum += (ann->weights[j][i] * sigma(ann->activation[j] + ann->bias[j]));
	}
	ann->y_dot[i] = (sum + (ann->sensor_weights[i] * sensor) - ann->activation[i]) / ann->time_constants[i];
    }
    for (i=0; i < NUM_NODES; i++) {
	ann->activation[i] = newActivation[i];
    }
}




// help modeling robot:
// http://rie2010.stuba.sk/pdf/41.pdf


double evaluate_chemotaxis (ANN *ann, int print) {
    int i, j;
    int step, rep;
    double x_pos;
    double y_pos;
    double sensor;
    double food_distance;
    double x_food;
    double y_food;
    double *velocity_left;
    double *velocity_right;
    double orientation;
    double wheel_radius;
    double linear_speed;
    double angular_speed;
    double chassis_radius;
    double sum_distance;
    double initial_distance;
    double fit;

    x_pos = 0;
    y_pos = 0;
    fit = 0;


    FILE *trace_fp;
    if (print) {
	trace_fp = fopen(TRACE_FILENAME, "w");
    }
    
    velocity_left = &ann->activation[0];
    velocity_right = &ann->activation[1];

    for (rep = 0; rep < NUM_CHEMOTAXIS_FOOD_REPS; rep++) {

	initial_distance = food_distance = (randf() * 5) + 10;
	double food_direction = randf() * 2 * PI;
	x_food = food_distance * sin(food_direction);
	y_food = food_distance * cos(food_direction);

	if (print) {
	    fprintf(trace_fp, "FOOD: %f %f\n", x_food, y_food);
	}

	sum_distance = 0;
	for (step=0; step<NUM_CHEMOTAXIS_STEPS; step++) {
	    //calculate sensor value
	    double x_diff = x_pos - x_food;
	    double y_diff = y_pos - y_food;
	    food_distance = sqrt( (x_diff * x_diff) + (y_diff * y_diff)); //pythagorean theorem

	    sum_distance += food_distance;

	    sensor = exp(LAMBDA * food_distance);

	    //update ann
	    update_ann(ann, sensor);

	    double radius = 0;
	    double distance = 0;

// MAYBE DO IT THIS WAY?
//	    if (*velocity_left > 0.0 && *velocity_right > 0.0) {
//		if (*velocity_left > *velocity_right) {
//		    circumference = (CHASSIS_RADIUS / 2) +
//			            (CHASSIS_RADIUS / (fmax(*velocity_left, *velocity_right) /
//						       fmin(*velocity_left, *velocity_right) - 1));


// I DON'T THINK THIS IS QUITE RIGHT
	    linear_speed = (*velocity_left + *velocity_right) / 2;
	    angular_speed = (*velocity_right - *velocity_left) / CHASSIS_RADIUS;
	    double delta_x = cos(orientation) * linear_speed;
	    double delta_y = sin(orientation) * linear_speed;

	    x_pos = x_pos + (delta_x * TIME_STEP);
	    y_pos = y_pos + (delta_y * TIME_STEP);
	    orientation = orientation + (angular_speed * TIME_STEP);

	    if (PRINT_ROBOT_INFO) {
		printf("x: %f  y: %f  t: %f  speed: %f\n", x_pos, y_pos, orientation, linear_speed); }
	    if (print) {
		fprintf(trace_fp, "%f %f\n", x_pos, y_pos);
	    }
	}
	fit += ( ( initial_distance - (sum_distance / NUM_CHEMOTAXIS_STEPS) ) / initial_distance );
    }

    if (print) {
	fclose(trace_fp);
    }
    return fit / NUM_CHEMOTAXIS_FOOD_REPS ;
}

double evaluate_insect (ANN *ann) {
    int step;
    double x;
    for (step=0; step < NUM_WALKING_STEPS; step++) {
    }
}

double evaluate_test (double *ind) {
    int i;
    double sum = 0;
    for (i = 0; i < NUM_NODES; i++) {
	sum += ind[i];
    }
    return (sum / NUM_NODES);
}


double evaluate(double *ind, int print) {

    //return evaluate_test(ind);
    int i;
    double fit, chemotaxis_fit, test_fit;
    ANN ann;

    decode_genome(ind, &ann);
    chemotaxis_fit = evaluate_chemotaxis(&ann, print);

    fit = chemotaxis_fit;
    return fit;
}
