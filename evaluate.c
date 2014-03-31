#include "defines.h"


double sigma (double x) {
    return (1 / (1 + exp(-x)));
}


void initialize_ann(ANN *ann) {
    int i;
    ann->bias = (double*)malloc(sizeof(double) * NUM_NODES);
    ann->time_constants = (double*)malloc(sizeof(double) * NUM_NODES);
    ann->activation = (double*)malloc(sizeof(double) * NUM_NODES);
    ann->sensor_weights = (double*)malloc(sizeof(double) * NUM_NODES);
    ann->weights = (double**)malloc(sizeof(double*) * NUM_NODES);
    for(i = 0; i < NUM_NODES; i++) {
	ann->weights[i] = (double*)malloc(sizeof(double) * NUM_NODES);
    }
}

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
	sum = 0;
	for (j=0; j < NUM_NODES; j++) {
	    sum += (ann->weights[j][i] * sigma(ann->activation[j] + ann->bias[j]));
	}
	double y_dot = (sum + (ann->sensor_weights[i] * sensor) - ann->activation[i]) / ann->time_constants[i];
	newActivation[i] = ann->activation[i] + (TIME_STEP * y_dot);
	//newActivation[i] = sum;
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
    //double velocity_left;
    //double velocity_right;
    //is this ok or do we need to map to a different range?
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

	food_distance = (randf() * 5) + 10;
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

	    if (step == 0) {
		initial_distance = food_distance;
	    }
	    sum_distance += food_distance;

	    //update ann
	    //normalize sensor between 0 and 1. not sure how izquierdo did this?
	    //sensor = food_distance / MAX_FOOD_DISTANCE;
	    sensor = food_distance / 50;
	    sensor = sensor > 1.0 ? 1.0 : sensor;
	    update_ann(ann, sensor);

	    //calculate dots
	    linear_speed = (*velocity_left + *velocity_right) / 2;
	    angular_speed = (*velocity_right - *velocity_left) / CHASSIS_RADIUS;
	    double x_dot = cos(orientation) * linear_speed;
	    double y_dot = sin(orientation) * linear_speed;

	    //update position and orientation
	    x_pos = x_pos + (x_dot * TIME_STEP);
	    y_pos = y_pos + (y_dot * TIME_STEP);
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
    return fit;
    //return 100 - food_distance;
}

double evaluate(double *ind, int print) {
    int i;
    double fit, chemotaxis_fit, test_fit;
    ANN ann;

    decode_genome(ind, &ann);
    chemotaxis_fit = evaluate_chemotaxis(&ann, print);

    fit = chemotaxis_fit;
    return fit;
}

double evaluate_insect (ANN *ann) {
    int step;
    double x;
    for (step=0; step < NUM_WALKING_STEPS; step++) {
    }

}
