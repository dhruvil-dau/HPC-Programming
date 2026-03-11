#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "init.h"
#include "utils.h"

int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

int particle_counts[5] = {100, 10000, 1000000, 100000000, 1000000000};

int main()
{
    NX = 250;
    NY = 100;
    Maxiter = 10;

    GRID_X = NX + 1;
    GRID_Y = NY + 1;
    dx = 1.0 / NX;
    dy = 1.0 / NY;

    printf("Particles,Interpolation_Time(sec)\n");

    for(int k=0; k<5; k++)
    {
        NUM_Points = particle_counts[k];

        double *mesh_value =
        (double*)calloc(GRID_X * GRID_Y, sizeof(double));

        Points *points =
        (Points*)calloc(NUM_Points, sizeof(Points));

        initializepoints(points);

        double interp_total = 0.0;

        for(int iter=0; iter<Maxiter; iter++)
        {
            clock_t start = clock();
            interpolation(mesh_value, points);
            clock_t end = clock();

            interp_total +=
            (double)(end - start) / CLOCKS_PER_SEC;
        }

        printf("%d,%lf\n", NUM_Points, interp_total);

        free(mesh_value);
        free(points);
    }

    return 0;
}