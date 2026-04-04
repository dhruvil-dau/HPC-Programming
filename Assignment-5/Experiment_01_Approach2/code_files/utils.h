#ifndef UTILS_HEADER_H
#define UTILS_HEADER_H

#include <time.h>
#include "init.h"

static inline double fast_rand(unsigned int *rng_state) {
    *rng_state = (*rng_state) * 1103515245u + 12345u;
    return (double)((*rng_state >> 16) & 0x7FFF) / 32767.0;
}

void interpolation(double *mesh_value, Points *points);

void mover_serial(Points *points, double deltaX, double deltaY);
void mover_parallel(Points *points, double deltaX, double deltaY);

void mover_serial_Approach1(Points *points, double deltaX, double deltaY);
void mover_parallel_Approach1(Points *points, double deltaX, double deltaY);

void mover_serial_Approach2(Points *points, double deltaX, double deltaY);
void mover_parallel_Approach2(Points *points, double deltaX, double deltaY);

void save_mesh(double *mesh_value);

#endif