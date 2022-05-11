#pragma once

#include "../common/octree.h"

/**
 * @brief Allocates an octree of the given capacity, on the GPU.
 *
 * @param capacity The capacity of the octree, in nodes.
 *
 * @return The allocated octree.
 */
Octree allocateOctreeCUDA(int capacity);

/**
 * @brief Resets the given Octree to it's initial state.
 *
 * @param tree The tree to reset.
 * @param capacity The capacity of the tree, in nodes.
 */
__device__ void resetOctreeGPU(Octree tree, int capacity);

/**
 * @brief Copies an octree allocated on CPU memory to one on GPU.
 *
 * @param cpuTree The CPU tree to copy.
 * @param cudaTree The GPU tree to copy to.
 * @param capacity The capacity of the trees.
 */
void copyOctreeToCUDA(Octree cpuTree, Octree cudaTree, int capacity);

/**
 * @brief Frees a tree
 *
 * @param root the root of the tree to be freed
 */
void freeTreeCUDA(Octree root);


/**
 * @brief Inserts an element into the octree that does not already exist.
 *
 * @param root The root of the octree.
 * @param node The node to insert under.
 * @param newVector The position of the new mass to add into the tree.
 * @param mass The new mass.
 */
__device__ void insertElementGPU(Octree root, int node, Vec3f newVector, float mass);

/**
 * @brief Converts a body into an octree pointer.
 *
 * @param root The root of the tree.
 * @param vector The vector of the position of the mass.
 * @param float The mass of the body.
 * @param x The X center point of this tree.
 * @param y The Y center point of this tree.
 * @param z The Z center point of this tree.
 * @param dist The radius of this section of the tree.
 *
 * @return int The index of the added node.
 */
__device__ int addOctreeChildGPU(Octree tree, Vec3f vector, float mass, float x, float y, float z, float dist);

/**
 * @brief Used when needing to subdivide an existing tree.
 *
 * @param root The tree needing to be divided.
 * @param node The node needed to be divided.
 * @param newBody The new body that is the reason for the divison.
 * @param mass The mass of the body.
 *
 * @return int The index of the now divided node.
 */
__device__ int subdivideOctreeGPU(Octree root, int node, Vec3f newBody, float mass);

/**
 * @brief Get the Octant of a vector.
 *
 * @param body The position of the body.
 * @param centerPosition The center position.
 *
 * @return int The relative octant.
 */
__device__ int getOctantVectorGPU(Vec3f position, Vec3f centerPosition);

/**
 * @brief Get the Octant of a body.
 *
 * @param position The position of the body being added.
 * @param x The center X position.
 * @param y The center Y position.
 * @param z The center Z position.
 *
 * @return int The relative octant.
 */
__device__ int getOctantPositionGPU(Vec3f position, float x, float y, float z);

/**
 * @brief Sets the maximum diameter of the octree.
 *
 * @param size The diameter of the octree.
 */
__device__ void setDiameterGPU(Octree tree, float size);
