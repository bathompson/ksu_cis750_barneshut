#include "cuda_octree.cuh"
#include "vector_utils.cuh"

#include <cuda_runtime_api.h>
#include <cuda.h>
#include <stdio.h>

Octree allocateOctreeCUDA(int capacity) {
    Octree tree;
    cudaMalloc(&tree.children, capacity * 8 * sizeof(int));
    cudaMemsetAsync(tree.children, -1, capacity * 8 * sizeof(int), 0);
    cudaMalloc(&tree.centerPosition, capacity * sizeof(Vec3f));
    cudaMemsetAsync(tree.centerPosition, 0, capacity * sizeof(Vec3f), 0);
    cudaMalloc(&tree.massPosition, capacity * sizeof(Vec3f));
    cudaMemsetAsync(tree.massPosition, 0, capacity * sizeof(Vec3f), 0);
    cudaMalloc(&tree.singleBody, capacity * sizeof(int));
    cudaMemsetAsync(tree.singleBody, 0, capacity * sizeof(int), 0);
    cudaMalloc(&tree.mass, capacity * sizeof(float));
    cudaMemsetAsync(tree.mass, 0, capacity * sizeof(float), 0);
    cudaMalloc(&tree.dist, capacity * sizeof(float));
    cudaMemsetAsync(tree.dist, 0, capacity * sizeof(float), 0);
    cudaMalloc(&tree.nextIndex, sizeof(int));
    cudaMemsetAsync(tree.nextIndex, 0, sizeof(int), 0);
    cudaDeviceSynchronize();
    return tree;
}

__device__ void resetOctreeGPU(Octree tree, int capacity) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    for (int i = index; i < capacity; i += blockDim.x * gridDim.x) {
        for (int j = 0; j < 8; j++)
            tree.children[i * 8 + j] = -1;
        tree.centerPosition[i] = newVec3fGPU(0, 0, 0);
        tree.massPosition[i] = newVec3fGPU(0, 0, 0);
        tree.singleBody[i] = 0;
        tree.mass[i] = 0;
        tree.dist[i] = 0;
        tree.nextIndex[i] = 0;
    }
}

void copyOctreeToCUDA(Octree cpuTree, Octree cudaTree, int capacity) {
    cudaMemcpyAsync(cudaTree.children, cpuTree.children, capacity * 8 * sizeof(int), cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.centerPosition, cpuTree.centerPosition, capacity * sizeof(Vec3f), cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.massPosition, cpuTree.massPosition, capacity * sizeof(Vec3f), cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.singleBody, cpuTree.singleBody, capacity * sizeof(int), cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.mass, cpuTree.mass, capacity * sizeof(float), cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.dist, cpuTree.dist, capacity * sizeof(float), cudaMemcpyHostToDevice, 0);
    cudaMemcpyAsync(cudaTree.nextIndex, cpuTree.nextIndex, sizeof(int), cudaMemcpyHostToDevice, 0);
    cudaDeviceSynchronize();
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

__device__ void insertElementGPU(Octree root, int node, Vec3f newVector, float mass) {
    //If tree does not exist yet.
    if(*root.nextIndex == 0) {
        addOctreeChildGPU(root, newVector, mass, 0, 0, 0, root.maxDiameter / 2);
    }

    //Get the octant it needs to be in.
    int octant = getOctantVectorGPU(newVector, root.centerPosition[node]);
    if(root.children[node * 8 + octant] == -1) {
        //If that octant is open, just insert and return root.
        //Increase mass of overall tree, calculate new sub octree and insert it.
        float rad = root.dist[node] / 2;
        float x = root.centerPosition[node].x + rad * (newVector.x > root.centerPosition[node].x ? 1 : -1);
        float y = root.centerPosition[node].y + rad * (newVector.y > root.centerPosition[node].y ? 1 : -1);
        float z = root.centerPosition[node].z + rad * (newVector.z > root.centerPosition[node].z ? 1 : -1);
        root.children[node * 8 + octant] = addOctreeChildGPU(root, newVector, mass, x, y, z, rad);
        root.singleBody[node] = 0;
        root.massPosition[node] = newVector;
        root.mass[node] = mass;
    } else if(root.singleBody[root.children[node * 8 + octant]] == 1) {
        //When there is already a single body there, subdivide the tree.
        Body retMass = combineMassGPU(root.massPosition[node], root.mass[node], newVector, mass);
        root.massPosition[node] = retMass.pos;
        root.mass[node] = retMass.mass;
        subdivideOctreeGPU(root, root.children[node * 8 + octant], newVector, mass);
    } else {
        //If octant is not a single body, and is not null, recursive call.
        Body retMass = combineMassGPU(root.massPosition[node], root.mass[node], newVector, mass);
        root.massPosition[node] = retMass.pos;
        root.mass[node] = retMass.mass;
        insertElementGPU(root, root.children[node * 8 + octant], newVector, mass);
    }
}

__device__ int addOctreeChildGPU(Octree tree, Vec3f vector, float mass, float x, float y, float z, float dist) {
    //Get the next node
    int childIndex = *tree.nextIndex;
    *tree.nextIndex = childIndex + 1;
    //We make the center of mass hold everything of the previous body.
    //Once it is no longer a single body, that data no longer matters.
    tree.mass[childIndex] = mass;
    tree.massPosition[childIndex] = vector;
    tree.centerPosition[childIndex].x = x;
    tree.centerPosition[childIndex].y = y;
    tree.centerPosition[childIndex].z = z;
    tree.dist[childIndex] = dist;
    tree.singleBody[childIndex] = 1;
    //return the index of the new node.
    return childIndex;
}

__device__ int subdivideOctreeGPU(Octree root, int node, Vec3f newBody, float mass) {
    int flag = 1;
    int rootOctant = 0;
    int newOctant = 0;
    int useThisToReturn = node;
    do {
        flag = 0;
        //Mark it as no longer a single body.
        root.singleBody[node] = 0;
        //Get the octant of the root and new body.
        rootOctant = getOctantVectorGPU(root.massPosition[node], root.centerPosition[node]);
        newOctant = getOctantVectorGPU(newBody, root.centerPosition[node]);
        if(rootOctant != newOctant) {
            root.children[node * 8 + rootOctant] = addOctreeChildGPU(root, root.massPosition[node], root.mass[node],
                                                                  root.centerPosition[node].x, root.centerPosition[node].y, root.centerPosition[node].z, root.dist[node]);
            root.children[node * 8 + newOctant] = addOctreeChildGPU(root, newBody, mass, root.centerPosition[node].x,
                                                                 root.centerPosition[node].y, root.centerPosition[node].z, root.dist[node]);
            flag = 0;
        } else {
            float distHalf = root.dist[node] / 2;
            float x = root.centerPosition[node].x + (distHalf * (newBody.x > root.centerPosition[node].x ? 1 : -1));
            float y = root.centerPosition[node].y + (distHalf * (newBody.y > root.centerPosition[node].y ? 1 : -1));
            float z = root.centerPosition[node].z + (distHalf * (newBody.z > root.centerPosition[node].z ? 1 : -1));
            int newChild = addOctreeChildGPU(root, root.massPosition[node], root.mass[node], x, y, z, distHalf);
            root.children[node * 8 + rootOctant] = newChild;
            node = newChild;
        }

        Body retBody = combineMassGPU(root.massPosition[node], root.mass[node], newBody, mass);
        root.massPosition[node] = retBody.pos;
        root.mass[node] = retBody.mass;
    } while(flag);

    return useThisToReturn;
}

__device__ int getOctantVectorGPU(Vec3f position, Vec3f centerPosition) {
    return getOctantPositionGPU(position, centerPosition.x, centerPosition.y, centerPosition.z);
}

__device__ int getOctantPositionGPU(Vec3f position, float x, float y, float z) {
    int ret = 0;
    if(position.x >= 0) {
        ret = position.y >= x ? 1 : 4;
    } else {
        ret = position.y >= y ? 2 : 3;
    }
    ret += position.z >= z ? 0 : 4;
    return ret - 1;
}

__device__ Octree setDiameterGPU(Octree tree, float maxSize) {
    tree.maxDiameter = maxSize;
    return tree;
}
