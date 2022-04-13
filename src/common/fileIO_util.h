#pragma once
#include <stdio.h>
#include "vector_utils.h"

void readInput(FILE *input, Body *loadLoc, size_t n);

void writeOutput(FILE *output, size_t xDim, size_t yDim, float *masses, Vec3f **pos);

void writeBodies(char* fileName, Body* bodies, size_t numBodies);