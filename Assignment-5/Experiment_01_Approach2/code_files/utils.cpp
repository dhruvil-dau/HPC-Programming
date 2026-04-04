#include "utils.h"
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void interpolation(double *mesh_value, Points *points) {
    memset(mesh_value, 0, (size_t)(GRID_X * GRID_Y) * sizeof(double));

    double inverse_dx = 1.0 / dx;
    double inverse_dy = 1.0 / dy;

    for (int idx = 0; idx < NUM_Points; ++idx) {
        double norm_x = points[idx].x * inverse_dx;
        double norm_y = points[idx].y * inverse_dy;

        int cell_x = (int)norm_x;
        int cell_y = (int)norm_y;

        cell_x = (cell_x >= NX) ? NX - 1 : ((cell_x < 0) ? 0 : cell_x);
        cell_y = (cell_y >= NY) ? NY - 1 : ((cell_y < 0) ? 0 : cell_y);

        double weight_x = norm_x - cell_x;
        double weight_y = norm_y - cell_y;

        int base_idx = cell_y * GRID_X + cell_x;
        mesh_value[base_idx] += (1.0 - weight_x) * (1.0 - weight_y);
        mesh_value[base_idx + 1] += weight_x * (1.0 - weight_y);
        mesh_value[base_idx + GRID_X] += (1.0 - weight_x) * weight_y;
        mesh_value[base_idx + GRID_X + 1] += weight_x * weight_y;
    }
}

void mover_serial(Points *points, double deltaX, double deltaY) {
    for (int k = 0; k < NUM_Points; ++k) {
        double rand_x = ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * deltaX;
        double rand_y = ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * deltaY;
        
        points[k].x += rand_x;
        points[k].y += rand_y;
        
        points[k].x = (points[k].x < 0.0) ? 0.0 : ((points[k].x > 1.0) ? 1.0 : points[k].x);
        points[k].y = (points[k].y < 0.0) ? 0.0 : ((points[k].y > 1.0) ? 1.0 : points[k].y);
    }
}

void mover_parallel(Points *points, double deltaX, double deltaY) {
#pragma omp parallel
    {
        unsigned int thread_seed = (unsigned int)(omp_get_thread_num() + 1) * 2654435761u;
#pragma omp for schedule(static)
        for (int k = 0; k < NUM_Points; ++k) {
            double rand_x = (fast_rand(&thread_seed) * 2.0 - 1.0) * deltaX;
            double rand_y = (fast_rand(&thread_seed) * 2.0 - 1.0) * deltaY;
            
            points[k].x += rand_x;
            points[k].y += rand_y;
            
            points[k].x = (points[k].x < 0.0) ? 0.0 : ((points[k].x > 1.0) ? 1.0 : points[k].x);
            points[k].y = (points[k].y < 0.0) ? 0.0 : ((points[k].y > 1.0) ? 1.0 : points[k].y);
        }
    }
}

void mover_serial_Approach1(Points *points, double deltaX, double deltaY) {
    int removed_count = 0;

    for (int k = 0; k < NUM_Points; ++k) {
        double rand_x = ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * deltaX;
        double rand_y = ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * deltaY;
        
        points[k].x += rand_x;
        points[k].y += rand_y;
        
        if (points[k].x < 0.0 || points[k].x > 1.0 || points[k].y < 0.0 || points[k].y > 1.0) {
            points[k].x = -1.0; 
            removed_count++;
        }
    }

    int write_pos = 0;
    for (int k = 0; k < NUM_Points; ++k) {
        if (points[k].x >= 0.0) {
            if (write_pos != k) {
                points[write_pos] = points[k];
            }
            write_pos++;
        }
    }

    for (int k = write_pos; k < NUM_Points; ++k) {
        points[k].x = rand() / (double)RAND_MAX;
        points[k].y = rand() / (double)RAND_MAX;
    }
}

void mover_serial_Approach2(Points *points, double deltaX, double deltaY) {
    for (int k = 0; k < NUM_Points; ++k) {
        double rand_x = ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * deltaX;
        double rand_y = ((rand() / (double)RAND_MAX) * 2.0 - 1.0) * deltaY;
        
        points[k].x += rand_x;
        points[k].y += rand_y;
        
        if (points[k].x < 0.0 || points[k].x > 1.0 || points[k].y < 0.0 || points[k].y > 1.0) {
            points[k].x = rand() / (double)RAND_MAX;
            points[k].y = rand() / (double)RAND_MAX;
        }
    }
}

void mover_parallel_Approach1(Points *points, double deltaX, double deltaY) {
    int max_t = omp_get_max_threads();

    int *global_free_slots = (int *)malloc((size_t)NUM_Points * sizeof(int));
    int *t_deleted = (int *)calloc(max_t, sizeof(int));
    int *t_offset = (int *)calloc(max_t + 1, sizeof(int));

#pragma omp parallel
    {
        int t_id = omp_get_thread_num();
        int n_threads = omp_get_num_threads();
        unsigned int thread_seed = (unsigned int)(t_id + 1) * 2654435761u;

        int chunk_size = NUM_Points / n_threads;
        int idx_start = t_id * chunk_size;
        int idx_end = (t_id == n_threads - 1) ? NUM_Points : idx_start + chunk_size;

        int deleted_local = 0;
        for (int k = idx_start; k < idx_end; ++k) {
            double rand_x = (fast_rand(&thread_seed) * 2.0 - 1.0) * deltaX;
            double rand_y = (fast_rand(&thread_seed) * 2.0 - 1.0) * deltaY;
            
            points[k].x += rand_x;
            points[k].y += rand_y;
            
            if (points[k].x < 0.0 || points[k].x > 1.0 || points[k].y < 0.0 || points[k].y > 1.0) {
                global_free_slots[idx_start + deleted_local] = k;
                deleted_local++;
            }
        }
        t_deleted[t_id] = deleted_local;
    }

    t_offset[0] = 0;
    for (int t = 0; t < max_t; ++t) {
        t_offset[t + 1] = t_offset[t] + t_deleted[t];
    }
    int agg_deleted = t_offset[max_t];

    int *compact_free_idx = (int *)malloc((size_t)agg_deleted * sizeof(int));
#pragma omp parallel
    {
        int t_id = omp_get_thread_num();
        int n_threads = omp_get_num_threads();
        int chunk_size = NUM_Points / n_threads;
        int idx_start = t_id * chunk_size;
        int dest_offset = t_offset[t_id];
        
        for (int j = 0; j < t_deleted[t_id]; ++j) {
            compact_free_idx[dest_offset + j] = global_free_slots[idx_start + j];
        }
    }

#pragma omp parallel
    {
        unsigned int thread_seed = (unsigned int)(omp_get_thread_num() + 1) * 1664525u;
#pragma omp for schedule(static)
        for (int k = 0; k < agg_deleted; ++k) {
            int target_idx = compact_free_idx[k];
            points[target_idx].x = fast_rand(&thread_seed);
            points[target_idx].y = fast_rand(&thread_seed);
        }
    }

    free(global_free_slots);
    free(t_deleted);
    free(t_offset);
    free(compact_free_idx);
}

void mover_parallel_Approach2(Points *points, double deltaX, double deltaY) {
#pragma omp parallel
    {
        unsigned int thread_seed = (unsigned int)(omp_get_thread_num() + 1) * 2654435761u;
#pragma omp for schedule(static)
        for (int k = 0; k < NUM_Points; ++k) {
            double rand_x = (fast_rand(&thread_seed) * 2.0 - 1.0) * deltaX;
            double rand_y = (fast_rand(&thread_seed) * 2.0 - 1.0) * deltaY;
            
            points[k].x += rand_x;
            points[k].y += rand_y;
            
            if (points[k].x < 0.0 || points[k].x > 1.0 || points[k].y < 0.0 || points[k].y > 1.0) {
                points[k].x = fast_rand(&thread_seed);
                points[k].y = fast_rand(&thread_seed);
            }
        }
    }
}

void save_mesh(double *mesh_value) {
    FILE *outFile = fopen("Mesh.out", "w");
    if (outFile == NULL) {
        printf("Error creating Mesh.out\n");
        exit(1);
    }
    for (int r = 0; r < GRID_Y; ++r) {
        for (int c = 0; c < GRID_X; ++c) {
            fprintf(outFile, "%lf ", mesh_value[r * GRID_X + c]);
        }
        fprintf(outFile, "\n");
    }
    fclose(outFile);
}