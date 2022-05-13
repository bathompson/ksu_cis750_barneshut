#include <stdlib.h>
#include <stdio.h>
#include <math.h>
extern "C"
{
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"
#include "../common/octree.h"
}
#include "vector_utils.cuh"
#include "cuda_octree.cuh"
#include <chrono>
#include <cooperative_groups.h>
namespace cg = cooperative_groups;

int G = 0;
int capcity = 0;

static const int blockSize = 32;

void __cudaCheck(cudaError err, const char* file, const int line);
#define cudaCheck(err) __cudaCheck (err, __FILE__, __LINE__)

void __cudaCheckLastError(const char* errorMessage, const char* file, const int line);
#define cudaCheckLastError(msg) __cudaCheckLastError (msg, __FILE__, __LINE__)

void __cudaCheck(cudaError err, const char *file, const int line)
{
    if( cudaSuccess != err) {
        fprintf(stderr, "%s(%i) : CUDA Runtime API error %d: %s.\n",
                file, line, (int)err, cudaGetErrorString( err ) );
        exit(-1);
    }
}

void __cudaCheckLastError(const char *errorMessage, const char *file, const int line)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) {
        fprintf(stderr, "%s(%i) : getLastCudaError() CUDA error : %s : (%d) %s.\n",
                file, line, errorMessage, (int)err, cudaGetErrorString( err ) );
        exit(-1);
    }
}

__device__ Vec3f computeBarnesHutForce(Octree root, int node, Body body, float theta, int G) {
    //Check if the root is null, if so return 0 valued vector.
    if(node == -1) {
        return newVec3fGPU(0,0,0);
    }
    //Calculate vector values.
    double scalarForce = 0;
    Vec3f vectorDist = ptToVectorGPU(body.pos, root.massPosition[node]);
    float distSq = vectorDotGPU(vectorDist, vectorDist);
    float invDist = __frsqrt_rn(distSq);
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

__device__ void findMaxSize(Body* bodies, int bodyCount, float *blockMax) {
    cg::grid_group grid = cg::this_grid();

    // Parallel reduction inspired by https://riptutorial.com/cuda/example/22458/multi-block-parallel-reduction-for-commutative-operator
    int threadIndex = threadIdx.x;
    int gridThreadIndex = threadIdx.x + blockIdx.x * blockDim.x;
    const int gridSize = blockDim.x * gridDim.x;

    // Compute max on this block
    float max = 0.0f;
    for (int i = gridThreadIndex; i < bodyCount; i += gridSize)
    {
        max = fmaxf(max, fabsf(bodies->pos.x));
        max = fmaxf(max, fabsf(bodies->pos.y));
        max = fmaxf(max, fabsf(bodies->pos.z));
    }
    __shared__ float shArr[blockSize];
    shArr[threadIndex] = max;
    __syncthreads();
    for (int size = blockSize / 2; size > 0; size /= 2)
    {
        if (threadIndex < size)
            shArr[threadIndex] = fmaxf(shArr[threadIndex], shArr[threadIndex + size]);
        __syncthreads();
    }
    if (threadIndex == 0)
        blockMax[blockIdx.x] = shArr[0];

    grid.sync();

    // If we are the first block, compute max from other blocks
    if (gridThreadIndex == 0) {
        shArr[threadIndex] = threadIndex < gridSize ? blockMax[threadIndex] : 0;
        __syncthreads();
        for (int size = blockSize / 2; size > 0; size /= 2) {
            if (threadIndex < size)
                shArr[threadIndex] = fmaxf(shArr[threadIndex], shArr[threadIndex + size]);
            __syncthreads();
        }
        if (threadIndex == 0)
            blockMax[0] = shArr[0];
    }
}

__global__ void constructBarnesHutTree(Octree *globalTree, Body *frame, int count, int capacity, float *blockMax) {
    cg::grid_group grid = cg::this_grid();

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    Octree tree = *globalTree;

    findMaxSize(frame, count, blockMax);
    resetOctreeGPU(tree, capacity);
    grid.sync();
    tree = setDiameterGPU(tree, blockMax[0]);

    if (index == 0) {
        for (size_t i = 0; i < count; i++) {
            insertElementGPU(tree, 0, frame[i].pos, frame[i].mass);
        }
    }

    if (index == 0) {
        *globalTree = tree;
    }
}

__global__ void simulateFrame(Octree *globalTree, Body *frame, int count, float t, float theta, int G) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < count) {
        Octree tree = *globalTree;
        Body body = frame[index];
        Vec3f netForce = computeBarnesHutForce(tree, 0, body, theta, G);
        Vec3f accel = vectorScalarMultGPU(1 / body.mass, netForce);
        body.mass = body.mass;
        body.pos = finalPosGPU(accel, body.vel, body.pos, t);
        body.vel = finalVelGPU(accel, body.vel, t);
        frame[index] = body;
    }
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

    Octree allocatedTree = allocateOctreeCUDA(capcity);
    Octree *gpuTree = nullptr;
    cudaMalloc(&gpuTree, sizeof(Octree));
    cudaMemcpy(gpuTree, &allocatedTree, sizeof(Octree), cudaMemcpyHostToDevice);

    float *gpuBlockMax = nullptr;
    cudaMalloc(&gpuBlockMax, sizeof(float));

    //Data input
    data = fopen(filePath, "r");
    readInput(data, frames[0], bodyCount);
    fclose(data);

    // Begin timing
    auto starttime = std::chrono::high_resolution_clock::now();

    // Copy initial frame of bodies to CUDA
    cudaMemcpy(gpuFrame, frames[0], bodyCount * sizeof(Body), cudaMemcpyHostToDevice);

    //Do the thing
    for(size_t i = 0; i < timeSteps - 1; i++) {
        // Build tree on GPU
        void *kernelArgs[] = {&gpuTree, &gpuFrame, &bodyCount, &capcity, &gpuBlockMax };
        cudaLaunchCooperativeKernel((void*)&constructBarnesHutTree, bodyCount, blockSize, kernelArgs);
        cudaDeviceSynchronize();

        // Simulate frame on GPU
        simulateFrame<<<(bodyCount + (blockSize - 1)) / blockSize, blockSize>>>(gpuTree, gpuFrame, bodyCount, deltaT, theta, G);
        cudaDeviceSynchronize();

        // Copy results back
        cudaMemcpy(frames[i + 1], gpuFrame, bodyCount * sizeof(Body), cudaMemcpyDeviceToHost);
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
    freeTreeCUDA(allocatedTree);
    cudaFree(gpuTree);
    cudaFree(gpuBlockMax);
}
