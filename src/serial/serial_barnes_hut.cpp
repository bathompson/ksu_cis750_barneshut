#include <stdlib.h>
#include <stdio.h>
#include <math.h>
extern "C"
{
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"
#include "../common/octree.h"
}
#include <chrono>

int G = 0;

Vec3f computeBarnesHutForce(Octree *root, Body *body, float theta) {
    //Check if the root is null, if so return 0 valued vector.
    if(root == NULL) {
        return newVec3f(0,0,0);
    }
    //Calculate vector values.
    double scalarForce = 0;
    Vec3f vectorDist = ptToVector(body->pos, root->massPosition);
    float distSq = vectorDot(vectorDist, vectorDist);
    float invDist = 1.0f/sqrtf(distSq);
    //Check if the body and root are equal, if so there is no forces
    if(vectorEq(body->pos, root->massPosition)) {
        return newVec3f(0, 0, 0);
    //Check either barnes hut condition or single body.
    } else if(2*root->dist*invDist < theta || root->singleBody) {
        //Find the force between two bodies
        scalarForce = G * body->mass * root->mass / distSq;
        return vectorScalarMult(scalarForce, vectorNormalize(vectorDist));
    } else {
        //Loop through all children
        Vec3f netForce = newVec3f(0,0,0);
        for(size_t i = 0; i < 8; i++) {
            netForce = vectorAdd(computeBarnesHutForce(root->bodies[i], body, theta), netForce);
        }
        return netForce;
    }
}

float findMaxSize(Body* bodies, int bodyCount) {
    float max = 0.0f;
    for(size_t i = 0; i < bodyCount; i++) {
        if(fabs(bodies->pos.x > max)) {
            max = fabs(bodies->pos.x);
        }
        if(fabs(bodies->pos.y > max)) {
            max = fabs(bodies->pos.y);
        }
        if(fabs(bodies->pos.z > max)) {
            max = fabs(bodies->pos.z);
        }
    }
    return max;
}

Octree *constructBarnesHutTree(Body *frame, size_t count) {
    Octree *tree = NULL;
    setDiameter(findMaxSize(frame, count));
    for(size_t i = 0; i<count; i++) {
        tree = insertElement(tree, frame[i].pos, frame[i].mass);
    }
    return tree;
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
    char *outputFileName;
    char *filePath;
    float *masses;
    float deltaT;
    float theta = 0.0f;

    //Handle input
    if(argc < 8) {
        printf("Usage: <path to input> <number of inputs> <number of timesteps> <length of time steps in seconds> <theta> <G> <outputFileName>\n");
        exit(0);
    }

    filePath = argv[1];
    bodyCount = strtoul(argv[2], NULL, 10);
    timeSteps = strtoul(argv[3], NULL, 10);
    deltaT = atof(argv[4]);
    theta = atof(argv[5]);
    G = atoi(argv[6]);
    outputFileName = argv[7];

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

    // Begin timing
    auto starttime = std::chrono::high_resolution_clock::now();

    //Do the thing
    for(size_t i = 0; i < timeSteps - 1; i++) {
        float t = i * deltaT;
        Vec3f netForce;
        root = constructBarnesHutTree(frames[i], bodyCount);
        for(size_t j = 0; j < bodyCount; j++) {
            netForce = computeBarnesHutForce(root, &frames[i][j], theta);
            Vec3f accel = vectorScalarMult(1/frames[i][j].mass, netForce);
            frames[i+1][j].mass = frames[i][j].mass;
            frames[i+1][j].pos = finalPos(accel, frames[i][j].vel, frames[i][j].pos, t);
            frames[i+1][j].vel = finalVel(accel, frames[i][j].vel, t);
        }
        freeTree(root);
    }

    // End timing
    auto endtime = std::chrono::high_resolution_clock::now();
    double runtime = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count() / 1000.0;
    printf("Simulated %d frames in %.4f seconds\n", timeSteps, runtime);

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
    out = fopen(outputFileName, "wb");
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