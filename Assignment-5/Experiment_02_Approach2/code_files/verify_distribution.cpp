#include "init.h"
#include "utils.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GRID_X, GRID_Y, NX, NY;
int NUM_Points, Maxiter;
double dx, dy;

void save_positions(Points *particles, int count, const char *fileName) {
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL) {
        printf("Error opening %s\n", fileName);
        return;
    }
    fprintf(fp, "x,y\n");
    for (int idx = 0; idx < count; ++idx) {
        fprintf(fp, "%.8f,%.8f\n", particles[idx].x, particles[idx].y);
    }
    fclose(fp);
    printf("Saved %d positions to %s\n", count, fileName);
}

int main() {
    NX = 250;
    NY = 100;
    GRID_X = NX + 1;
    GRID_Y = NY + 1;
    dx = 1.0 / (double)NX;
    dy = 1.0 / (double)NY;
    NUM_Points = 100000;
    Maxiter = 5;

    double *gridMesh = (double *)calloc((size_t)(GRID_X * GRID_Y), sizeof(double));
    Points *particleArray = (Points *)calloc((size_t)NUM_Points, sizeof(Points));

    srand(42);
    initializepoints(particleArray);
    save_positions(particleArray, NUM_Points, "positions_initial.csv");

    srand(42);
    initializepoints(particleArray);
    for (int step = 0; step < Maxiter; ++step) {
        interpolation(gridMesh, particleArray);
        mover_serial_Approach1(particleArray, dx, dy);
    }
    save_positions(particleArray, NUM_Points, "positions_after_Approach1.csv");

    srand(42);
    initializepoints(particleArray);
    for (int step = 0; step < Maxiter; ++step) {
        interpolation(gridMesh, particleArray);
        mover_serial_Approach2(particleArray, dx, dy);
    }
    save_positions(particleArray, NUM_Points, "positions_after_Approach2.csv");

    save_mesh(gridMesh);
    printf("Mesh saved to Mesh.out\n");

    free(gridMesh);
    free(particleArray);
    return 0;
}