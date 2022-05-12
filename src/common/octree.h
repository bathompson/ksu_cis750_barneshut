#pragma once
#include "vector_utils.h"

typedef struct Octree {
    //See https://upload.wikimedia.org/wikipedia/commons/thumb/6/60/Octant_numbers.svg/1200px-Octant_numbers.svg.png for octant labeling.
    int* children;
    Vec3f* centerPosition;
    Vec3f* massPosition;
    int* singleBody;
    float* mass;
    float* dist;
    int* nextIndex;
    float maxDiameter;
} Octree;

/**
 * @brief Allocates an octree of the given capacity.
 *
 * @param capacity The capacity of the octree, in nodes.
 *
 * @return The allocated octree.
 */
Octree allocateOctreeCPU(int capacity);

/**
 * @brief Resets the given Octree to it's initial state.
 *
 * @param tree The tree to reset.
 * @param capacity The capacity of the tree, in nodes.
 */
void resetOctreeCPU(Octree tree, int capacity);

/**
 * @brief Inserts an element into the octree that does not already exist.
 * 
 * @param root The root of the octree.
 * @param node The node to insert under.
 * @param newVector The position of the new mass to add into the tree.
 * @param mass The new mass.
 */
int insertElement(Octree root, int node, Vec3f newVector, float mass);

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
int addOctreeChild(Octree tree, Vec3f vector, float mass, float x, float y, float z, float dist);

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
int subdivideOctree(Octree root, int node, Vec3f newBody, float mass);

/**
 * @brief Get the Octant of a vector.
 * 
 * @param body The position of the body.
 * @param centerPosition The center position. 
 * 
 * @return int The relative octant.
 */
int getOctantVector(Vec3f position, Vec3f centerPosition);

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
int getOctantPosition(Vec3f position, float x, float y, float z);

/**
 * @brief Frees a tree
 * 
 * @param root the root of the tree to be freed
 */
void freeTreeCPU(Octree root);

/**
 * @brief Sets the maximum diameter of the octree.
 * 
 * @param size The diameter of the octree.
 */
Octree setDiameter(Octree tree, float size);

/**
 * @brief Debug method to make sure things are inserting properly.
 * 
 * @param root The tree.
 * @param leadingSpaces the amount of spaces to lead the prints with (used to see children easier).
 */
void _debugPrint(Octree root, int node, int leadingSpaces);