#ifndef INIT_HEADER_H
#define INIT_HEADER_H

#include <stdio.h>

typedef struct {
    double x;
    double y;
} Points;

extern int GRID_X, GRID_Y, NX, NY;
extern int NUM_Points, Maxiter;
extern double dx, dy;

void initializepoints(Points *points);

#endif