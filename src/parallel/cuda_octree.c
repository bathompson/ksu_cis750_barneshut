#include "cuda_octree.h"

#include <cuda_runtime_api.h>
#include <cuda.h>

Octree allocateOctreeCUDA(int capacity) {
    Octree tree;
    cudaMalloc(&tree.children, capacity * 8 * sizeof(int));
    cudaMemsetAsync(tree.children, -1, capacity * 8, 0);
    cudaMalloc(&tree.centerPosition, capacity * sizeof(Vec3f));
    cudaMemsetAsync(tree.centerPosition, 0, capacity, 0);
    cudaMalloc(&tree.massPosition, capacity * sizeof(Vec3f));
    cudaMemsetAsync(tree.massPosition, 0, capacity, 0);
    cudaMalloc(&tree.singleBody, capacity * sizeof(int));
    cudaMemsetAsync(tree.singleBody, 0, capacity, 0);
    cudaMalloc(&tree.mass, capacity * sizeof(float));
    cudaMemsetAsync(tree.mass, 0, capacity, 0);
    cudaMalloc(&tree.dist, capacity * sizeof(float));
    cudaMemsetAsync(tree.dist, 0, capacity, 0);
    cudaMalloc(&tree.nextIndex, sizeof(int));
    cudaMemsetAsync(tree.nextIndex, 0, capacity, 0);
    cudaDeviceSynchronize();
    return tree;
}

void resetOctreeCUDA(Octree tree, int capacity) {
    cudaMemsetAsync(tree.children, -1, capacity * 8, 0);
    cudaMemsetAsync(tree.centerPosition, 0, capacity, 0);
    cudaMemsetAsync(tree.massPosition, 0, capacity, 0);
    cudaMemsetAsync(tree.singleBody, 0, capacity, 0);
    cudaMemsetAsync(tree.mass, 0, capacity, 0);
    cudaMemsetAsync(tree.dist, 0, capacity, 0);
    cudaMemsetAsync(tree.nextIndex, 0, capacity, 0);
    cudaDeviceSynchronize();
}

void copyOctreeToCUDA(Octree cpuTree, Octree cudaTree, int capacity) {
    cudaMemcpyAsync(cudaTree.children, cpuTree.children, capacity * 8, cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.centerPosition, cpuTree.centerPosition, capacity, cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.massPosition, cpuTree.massPosition, capacity, cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.singleBody, cpuTree.singleBody, capacity, cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.mass, cpuTree.mass, capacity, cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.dist, cpuTree.dist, capacity, cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.nextIndex, cpuTree.nextIndex, capacity, cudaMemcpyHostToDevice, 0);
}

void freeTreeCUDA(Octree root) {
    cudaFree(root.children);
    cudaFree(root.centerPosition);
    cudaFree(root.massPosition);
    cudaFree(root.singleBody);
    cudaFree(root.mass);
    cudaFree(root.dist);
    cudaFree(root.nextIndex);
}
