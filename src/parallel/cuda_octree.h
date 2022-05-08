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
void resetOctreeCUDA(Octree tree, int capacity);

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
