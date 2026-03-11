#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "init.h"
#include "utils.h"

int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

int main()
{
    NUM_Points = 100000000;
    Maxiter = 10;

    int Nx_list[3] = {250,500,1000};
    int Ny_list[3] = {100,200,400};

    omp_set_num_threads(4);

    printf("Problem_Index,Interpolation_Time(sec)\n");

    for(int p=0;p<3;p++)
    {
        NX = Nx_list[p];
        NY = Ny_list[p];

        GRID_X = NX+1;
        GRID_Y = NY+1;

        dx = 1.0/NX;
        dy = 1.0/NY;

        double *mesh_value =
        (double*)calloc(GRID_X*GRID_Y,sizeof(double));

        Points *points =
        (Points*)calloc(NUM_Points,sizeof(Points));

        initializepoints(points);

        double interp_total = 0.0;

        for(int iter=0;iter<Maxiter;iter++)
        {
            clock_t s=clock();
            interpolation(mesh_value,points);
            clock_t e=clock();

            interp_total +=
            (double)(e-s)/CLOCKS_PER_SEC;
        }

        printf("%d,%lf\n",p+1,interp_total);

        free(mesh_value);
        free(points);
    }

    return 0;
}