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
    NUM_Points = 14000000;
    int grid_configs[][2] = {{250, 100}, {500, 200}, {1000, 400}};
    int num_threads_list[] = {2, 4, 8, 16};
    int total_threads = 4;
    int total_grids = 3;

    printf("=== Experiment 2: Approach2 (Immediate Replacement) – Parallel Scalability ===\n");
    printf("NX,NY,Threads,Serial_Mover,Parallel_Mover,Speedup,Serial_Total,Parallel_Total\n");

    for (int grid_idx = 0; grid_idx < total_grids; ++grid_idx) {
        NX = grid_configs[grid_idx][0]; 
        NY = grid_configs[grid_idx][1];
        GRID_X = NX + 1;  
        GRID_Y = NY + 1;
        dx = 1.0 / (double)NX;    
        dy = 1.0 / (double)NY;

        double *grid_mesh = (double *)calloc((size_t)(GRID_X * GRID_Y), sizeof(double));
        Points *particles = (Points *)calloc((size_t)NUM_Points, sizeof(Points));
        
        if (grid_mesh == NULL || particles == NULL) { 
            printf("OOM\n"); 
            return 1; 
        }

        srand(42);
        initializepoints(particles);

        double s_interp = 0.0;
        double s_mover = 0.0;
        
        for (int iter_step = 0; iter_step < Maxiter; ++iter_step) {
            double start_time = omp_get_wtime();
            interpolation(grid_mesh, particles);
            double mid_time = omp_get_wtime();
            mover_serial_Approach2(particles, dx, dy);
            double end_time = omp_get_wtime();
            
            s_interp += (mid_time - start_time);
            s_mover += (end_time - mid_time);
        }
        
        double s_total = s_interp + s_mover;
        free(grid_mesh); 
        free(particles);

        for (int th_idx = 0; th_idx < total_threads; ++th_idx) {
            omp_set_num_threads(num_threads_list[th_idx]);

            grid_mesh = (double *)calloc((size_t)(GRID_X * GRID_Y), sizeof(double));
            particles = (Points *)calloc((size_t)NUM_Points, sizeof(Points));
            
            srand(42);
            initializepoints(particles);

            double p_interp = 0.0;
            double p_mover = 0.0;
            
            for (int iter_step = 0; iter_step < Maxiter; ++iter_step) {
                double start_time = omp_get_wtime();
                interpolation(grid_mesh, particles);
                double mid_time = omp_get_wtime();
                mover_parallel_Approach2(particles, dx, dy);
                double end_time = omp_get_wtime();
                
                p_interp += (mid_time - start_time);
                p_mover += (end_time - mid_time);
            }
            
            double p_total = p_interp + p_mover;
            double speedup_ratio = s_mover / p_mover;

            printf("%d,%d,%d,%lf,%lf,%lf,%lf,%lf\n", NX, NY, num_threads_list[th_idx], s_mover, p_mover, speedup_ratio, s_total, p_total);
            
            free(grid_mesh); 
            free(particles);
        }
    }
    return 0;
}