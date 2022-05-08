#include <stdlib.h>
#include <stdio.h>
#include <math.h>
extern "C"
{
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"
#include "../common/octree.h"
#include "cuda_octree.h"
}
#include "vector_utils.cuh"
#include <chrono>

int G = 0;
int capcity = 0;

__device__ Vec3f computeBarnesHutForce(Octree root, int node, Body body, float theta, int G) {
    //Check if the root is null, if so return 0 valued vector.
    if(node == -1) {
        return newVec3fGPU(0,0,0);
    }
    //Calculate vector values.
    double scalarForce = 0;
    Vec3f vectorDist = ptToVectorGPU(body.pos, root.massPosition[node]);
    float distSq = vectorDotGPU(vectorDist, vectorDist);
    float invDist = 1.0f/sqrtf(distSq);
    //Check if the body and root are equal, if so there is no forces
    if(vectorEqGPU(body.pos, root.massPosition[node])) {
        return newVec3fGPU(0, 0, 0);
        //Check either barnes hut condition or single body.
    } else if(2*root.dist[node]*invDist < theta || root.singleBody[node]) {
        //Find the force between two bodies
        scalarForce = G * body.mass * root.mass[node] / distSq;
        return vectorScalarMultGPU(scalarForce, vectorNormalizeGPU(vectorDist));
    } else {
        //Loop through all children
        Vec3f netForce = newVec3fGPU(0,0,0);
        for(size_t i = 0; i < 8; i++) {
            netForce = vectorAddGPU(computeBarnesHutForce(root, root.children[node * 8 + i], body, theta, G), netForce);
        }
        return netForce;
    }
}

__global__ void simulateFrame(Octree tree, Body* frame, int count, float t, float theta, int G) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < count) {
        Body body = frame[index];
        Vec3f netForce = computeBarnesHutForce(tree, 0, body, theta, G);
        Vec3f accel = vectorScalarMultGPU(1 / body.mass, netForce);
        body.mass = body.mass;
        body.pos = finalPosGPU(accel, body.vel, body.pos, t);
        body.vel = finalVelGPU(accel, body.vel, t);
        frame[index] = body;
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

void constructBarnesHutTree(Octree tree, Body *frame, size_t count) {
    resetOctreeCPU(tree, capcity);
    setDiameter(findMaxSize(frame, count));
    for(size_t i = 0; i<count; i++) {
        insertElement(tree, 0, frame[i].pos, frame[i].mass);
    }
    //_debugPrint(tree, 0, 2);
}

int main(int argc, char **argv) {
    //Declarations
    size_t bodyCount;
    size_t timeSteps;
    FILE *data;
    FILE *out;
    Body **frames;
    Body *gpuFrame;
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

    cudaMalloc(&gpuFrame, bodyCount * sizeof(Body));

    masses = (float *) malloc(bodyCount * sizeof(float));
    pos = (Vec3f **) malloc(timeSteps * sizeof(Vec3f *));
    for(size_t i = 0; i < timeSteps; i++) {
        pos[i] = (Vec3f *) malloc(bodyCount * sizeof(Vec3f));
    }

    capcity = bodyCount * 2;
    Octree tree = allocateOctreeCPU(capcity);
    Octree gpuTree = allocateOctreeCUDA(capcity);

    //Data input
    data = fopen(filePath, "r");
    readInput(data, frames[0], bodyCount);
    fclose(data);

    // Begin timing
    auto starttime = std::chrono::high_resolution_clock::now();

    //Do the thing
    for(size_t i = 0; i < timeSteps - 1; i++) {
        // Build tree and copy it to CUDA
        constructBarnesHutTree(tree, frames[i], bodyCount);
        copyOctreeToCUDA(tree, gpuTree, capcity);

        // Copy input frame of bodies to CUDA
        cudaMemcpy(gpuFrame, frames[i], bodyCount, cudaMemcpyHostToDevice);

        // Simulate frame on GPU
        simulateFrame<<<(bodyCount + 255) / 256, 256>>>(gpuTree, gpuFrame, bodyCount, deltaT, theta, G);

        // Copy results back
        cudaMemcpy(frames[i + 1], gpuFrame, bodyCount, cudaMemcpyDeviceToHost);
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
    cudaFree(gpuFrame);

    freeTreeCPU(tree);
    freeTreeCUDA(gpuTree);
}