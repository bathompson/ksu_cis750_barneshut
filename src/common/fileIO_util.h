#pragma once
#include <stdio.h>
#include "vector_utils.h"

/**
 * @brief Reads an input file formatted from the data generator.
 * 
 * @param input The input file to read.
 * @param loadLoc The location to store the data into.
 * @param n The number of data points to read.
 */
void readInput(FILE *input, Body *loadLoc, size_t n);

/**
 * @brief Writes the output to a .nbody file.
 * 
 * @param output The output file to write to.
 * @param xDim The X dimension.
 * @param yDim The Y dimension.
 * @param masses Pointer to the masses.
 * @param pos Double pointer to the positions.
 */
void writeOutput(FILE *output, size_t xDim, size_t yDim, float *masses, Vec3f **pos);

/**
 * @brief Writes the generated bodies out to a csv file.
 * 
 * @param fileName The name of the output file to write to.
 * @param bodies The bodies to write.
 * @param numBodies The number of bodies to write.
 */
void writeBodies(char* fileName, Body* bodies, size_t numBodies);