#pragma once
#include "vector_utils.h"

typedef struct Octree {
    //See https://upload.wikimedia.org/wikipedia/commons/thumb/6/60/Octant_numbers.svg/1200px-Octant_numbers.svg.png for octant labeling.
    struct Octree* bodies[8];
    Vec3f centerPosition;
    Vec3f massPosition;
    int singleBody;
    float mass;
    float dist;
} Octree;

void setDiameter(float size);

/**
 * @brief Inserts an element into the octree that does not already exist.
 * 
 * @param root The root of the octree.
 * @param newVector The positon of the new mass to add into the tree.
 * @param mass The new mass.
 * 
 * @return Octree* The root of the octree.
 */
Octree* insertElement(Octree* root, Vec3f newVector, float mass);

/**
 * @brief Converts a body into an octree pointer.
 * 
 * @param vector The vector of the position of the mass.
 * @param float The mass of the body.
 * @param x The X center point of this tree.
 * @param y The Y center point of this tree.
 * @param z The Z center point of this tree.
 * @param dist The radius of this section of the tree.
 * 
 * @return Octree* The single celled octree.
 */
Octree* vectorToOctree(Vec3f vector, float mass, float x, float y, float z, float dist);

/**
 * @brief Used when needing to subdivide an existing tree.
 * 
 * @param rootTree The tree needing to be divided.
 * @param newBody The new body that is the reason for the divison.
 * @param mass The mass of the body.
 * 
 * @return Octree* The pointer to the now divided tree.
 */
Octree* subdivideOctree(Octree* rootTree, Vec3f newBody, float mass);

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
 * @brief Debug method to make sure things are inserting properly.
 * 
 * @param root The tree.
 * @param leadingSpaces the amount of spaces to lead the prints with (used to see children easier).
 */
void _debugPrint(Octree* root, int leadingSpaces);

/**
 * @brief Frees a tree
 * 
 * @param root the root of the tree to be freed
 */
void freeTree(Octree *root);

