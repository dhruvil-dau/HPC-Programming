#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "init.h"

void initializepoints(Points *points) {
    for (int idx = 0; idx < NUM_Points; ++idx) {
        points[idx].x = rand() / (double)RAND_MAX;
        points[idx].y = rand() / (double)RAND_MAX;
    }
}