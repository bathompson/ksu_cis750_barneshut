#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "vector_utils.h"
#include "fileIO_util.h"
#include "../common/octree.h"

#define G 6.67430e-11

Vec3f computeBarnesHutForce(Octree *root, Body *body, float theta)
{
    double scalarForce = 0;
    Vec3f vectorDist = ptToVector(body->pos, root->centerOfMass.pos);
    float distSq = vectorDot(vectorDist, vectorDist);
    float invDist = 1/sqrt(distSq);
    if(2*root->rad*invDist < theta || root->singleBody)
    {
        //Find the force between two bodies
        scalarForce = G * body->mass * root->centerOfMass.mass / distSq;
        return vectorScalarMult(scalarForce, vectorNormalize(vectorDist));
    }
    else
    {
        Vec3f netForce = newVec3f(0,0,0);
        for(size_t i = 0; i<8; i++)
        {
            netForce = vectorAdd(computeBarnesHutForce(root->bodies[i], body, theta), netForce);
        }
        return netForce;
    }
}

int main(int argc, char **argv) {
    //Declarations
    size_t bodyCount;
    size_t timeSteps;
    FILE *data;
    FILE *out;
    Body **frames;
    Vec3f **pos;
    Octree *root;
    char *filePath;
    float *masses;
    float deltaT;
    float theta = 0.5;

    //Handle input
    if(argc < 5) {
        printf("Usage: <path to input> <number of inputs> <number of timesteps> <length of time steps in seconds>\n");
        exit(0);
    }

    filePath = argv[1];
    bodyCount = strtoul(argv[2], NULL, 10);
    timeSteps = strtoul(argv[3], NULL, 10);
    deltaT = atof(argv[4]);

    //Allocate Memory
    frames = (Body **) malloc(timeSteps * sizeof(Body *));

    for(size_t i = 0; i < timeSteps; i++) {
        frames[i] = (Body *) malloc(bodyCount * sizeof(Body));
    }

    masses = (float *) malloc(bodyCount * sizeof(float));
    pos = (Vec3f **) malloc(timeSteps * sizeof(Vec3f *));
    for(size_t i = 0; i < timeSteps; i++) {
        pos[i] = (Vec3f *) malloc(bodyCount * sizeof(Vec3f));
    }

    //Data input
    data = fopen(filePath, "r");
    readInput(data, frames[0], bodyCount);
    fclose(data);

    //Do the thing
    for(size_t i = 0; i < timeSteps - 1; i++) 
    {
        float t = i * deltaT;
        Vec3f netForce;
        for(size_t j = 0; j < bodyCount; j++) 
        {
            netForce = computeBarnesHutForce(root, &frames[i][j], theta);
            Vec3f accel = vectorScalarMult(1/frames[i][j].mass, netForce);
            frames[i+1][j].mass = frames[i][j].mass;
            frames[i+1][j].pos = finalPos(netForce, frames[i][j].vel, frames[i][j].pos, t);
            frames[i+1][j].vel = finalVel(netForce, frames[i][j].vel, t);
        }
    }

    //Put data into output format
    for(size_t i = 0; i < bodyCount; i++) {
        masses[i] = frames[0][i].mass;
    }

    for(size_t i = 0; i < timeSteps; i++) {
        for(size_t j = 0; j < bodyCount; j++) {
            pos[i][j] = frames[i][j].pos;
        }
    }

    //Write to output
    out = fopen("out.nbody", "wb");
    writeOutput(out, bodyCount, timeSteps, masses, pos);
    fclose(out);

    //Free memory
    for(size_t i = 0; i < timeSteps; i++) {
        free(frames[i]);
        free(pos[i]);
    }

    free(frames);
    free(pos);
    free(masses);
}