#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "init.h"
#include "utils.h"

int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

int main() {
    Maxiter = 10;
    int grid_configs[][2] = {{250, 100}, {500, 200}, {1000, 400}};
    int p_counts[] = {100, 10000, 1000000, 100000000};
    int total_pc = 4; 
    int total_grids = 3;

    printf("=== Experiment 1: Approach1 (Deferred Insertion) (Serial) ===\n");
    printf("NX,NY,Particles,Interp_Time,Mover_Time,Total_Time\n");

    for (int grid_idx = 0; grid_idx < total_grids; ++grid_idx) {
        NX = grid_configs[grid_idx][0]; 
        NY = grid_configs[grid_idx][1];
        GRID_X = NX + 1;  
        GRID_Y = NY + 1;
        dx = 1.0 / (double)NX;    
        dy = 1.0 / (double)NY;

        for (int part_idx = 0; part_idx < total_pc; ++part_idx) {
            NUM_Points = p_counts[part_idx];

            double *grid_mesh = (double *)calloc((size_t)(GRID_X * GRID_Y), sizeof(double));
            Points *particles = (Points *)calloc((size_t)NUM_Points, sizeof(Points));
            
            if (grid_mesh == NULL || particles == NULL) {
                printf("%d,%d,%d,OOM,OOM,OOM\n", NX, NY, NUM_Points);
                if (grid_mesh) free(grid_mesh);
                if (particles) free(particles);
                continue;
            }

            srand(42);
            initializepoints(particles);

            double interp_dur = 0.0;
            double mover_dur = 0.0;

            for (int iter_step = 0; iter_step < Maxiter; ++iter_step) {
                double start_time = omp_get_wtime();
                interpolation(grid_mesh, particles);
                double mid_time = omp_get_wtime();
                mover_serial_Approach1(particles, dx, dy);
                double end_time = omp_get_wtime();
                
                interp_dur += (mid_time - start_time);
                mover_dur += (end_time - mid_time);
            }
            
            printf("%d,%d,%d,%lf,%lf,%lf\n", NX, NY, NUM_Points, interp_dur, mover_dur, interp_dur + mover_dur);
            
            free(grid_mesh); 
            free(particles);
        }
    }
    return 0;
}