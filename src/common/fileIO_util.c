#include "fileIO_util.h"

void readInput(FILE *input, Body *loadLoc, size_t n) {
    float loadMass;
    float loadPosX, loadPosY, loadPosZ;
    float loadVelX, loadVelY, loadVelZ;
    for(size_t i = 0; i<n; i++) {
        fscanf(input, "%f,%f,%f,%f,%f,%f,%f\n", &loadMass, &loadPosX, &loadPosY, &loadPosZ, &loadVelX, &loadVelY, &loadVelZ);
        loadLoc[i].mass = loadMass;
        loadLoc[i].pos = newVec3f(loadPosX, loadPosY, loadPosZ);
        loadLoc[i].vel = newVec3f(loadVelX, loadVelY, loadVelZ);
    }
}

void writeOutput(FILE *output, size_t xDim, size_t yDim, float *masses, Vec3f **pos) {
    fwrite(&xDim, sizeof(size_t), 1, output);
    fwrite(masses, sizeof(float), xDim, output);
    for(size_t i = 0; i<yDim; i++) {
        fwrite(pos[i], sizeof(Vec3f), xDim, output);
    }
}

void writeBodies(char* output, Body* bodies, size_t numBodies) {
    FILE* outputFile = fopen(output, "w");
    //Print out the data.
    for(int i = 0; i < numBodies; i++) {
        fprintf(outputFile, "%f,%f,%f,%f,%f,%f,%f\n", bodies[i].mass, bodies[i].pos.x, bodies[i].pos.y, bodies[i].pos.z, 
            bodies[i].vel.x, bodies[i].vel.y, bodies[i].vel.z);
            
    }

    fclose(outputFile);
}