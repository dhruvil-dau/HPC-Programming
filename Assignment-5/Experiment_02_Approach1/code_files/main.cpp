#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "init.h"
#include "utils.h"

int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

int main(int argc, char **argv) {

    NX = 200;
    NY = 200;
    Maxiter = 10;
    NUM_Points = 100000000;

    GRID_X = NX + 1;
    GRID_Y = NY + 1;
    dx = 1.0 / (double)NX;
    dy = 1.0 / (double)NY;

    omp_set_num_threads(4);

    double *meshArray = (double *)calloc((size_t)(GRID_X * GRID_Y), sizeof(double));
    Points *particleData = (Points *)calloc((size_t)NUM_Points, sizeof(Points));

    initializepoints(particleData);

    printf("Iter\tInterp\t\tMover\t\tTotal\n");
    for (int step = 0; step < Maxiter; ++step) {

        clock_t t_interp_start = clock();
        interpolation(meshArray, particleData);
        clock_t t_interp_end = clock();

        clock_t t_move_start = clock();
        mover_serial(particleData, dx, dy);
        clock_t t_move_end = clock();

        double t_interp = (double)(t_interp_end - t_interp_start) / CLOCKS_PER_SEC;
        double t_move = (double)(t_move_end - t_move_start) / CLOCKS_PER_SEC;
        double t_total = t_interp + t_move;

        printf("%d\t%lf\t%lf\t%lf\n", step + 1, t_interp, t_move, t_total);
    }

    free(meshArray);
    free(particleData);

    return 0;
}