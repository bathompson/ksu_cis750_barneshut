#pragma once
#include <stdio.h>
#include "VectorUtils.h"

void readInput(FILE *input, Body *loadLoc, size_t n);

void writeOutput(FILE *output, size_t xDim, size_t yDim, float *masses, Vec3f **pos);
