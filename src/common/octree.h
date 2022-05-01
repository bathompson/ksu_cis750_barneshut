#pragma once
#include "vector_utils.h"

typedef struct Octree {
    //See https://upload.wikimedia.org/wikipedia/commons/thumb/6/60/Octant_numbers.svg/1200px-Octant_numbers.svg.png for octant labeling.
    struct Octree* bodies[8];
    float rad;
    Body centerOfMass;
    Vec3f centerPosition;
    int singleBody;
} Octree;

/**
 * @brief Inserts an element into the octree that does not already exist.
 * 
 * @param root The root of the octree.
 * @param newBody The new body to insert into the octree.
 * @return Octree* The root of the octree.
 */
Octree* insertElement(Octree* root, Body newBody);

/**
 * @brief Converts a body into an octree pointer.
 * 
 * @param body The body to convert.
 * @param x The X center point of this tree.
 * @param y The Y center point of this tree.
 * @param z The Z center point of this tree.
 * @param rad The radius of this section of the tree.
 * 
 * @return Octree* The single celled octree.
 */
Octree* bodyToOctree(Body body, float x, float y, float z, float rad);

/**
 * @brief Used when needing to subdivide an existing tree.
 * 
 * @param rootTree The tree needing to be divided.
 * @param newBody The new body that is the reason for the divison.
 * @return Octree* The pointer to the now divided tree.
 */
Octree* subdivideOctree(Octree* rootTree, Body newBody);

/**
 * @brief Get the Octant of a body.
 * 
 * @param body The body.
 * @param centerPosition The center position. 
 * @return int The relative octant.
 */
int getOctantVector(Body body, Vec3f centerPosition);

/**
 * @brief Get the Octant of a body.
 * 
 * @param body The body.
 * @param x The center X position.
 * @param y The center Y position.
 * @param z The center Z position.
 * @return int The relative octant.
 */
int getOctantPosition(Body body, int x, int y, int z);

/**
 * @brief Debug method to make sure things are inserting properly.
 * 
 * @param root The tree.
 * @param leadingSpaces the amount of spaces to lead the prints with (used to see children easier).
 */
void _debugPrint(Octree* root, int leadingSpaces);