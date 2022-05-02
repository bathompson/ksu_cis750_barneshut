#include <stdlib.h>
#include <stdio.h>
#include "vector_utils.h"
#include "octree.h"

#define MAX_TREE_DIAMETER __FLT_MAX__
Octree* insertElement(Octree* root, Vec3f newVector, float mass) {
    //If tree does not exist.
    if(root == NULL) {
        //Create tree and set all values to be defaults.
        Octree* retTree = vectorToOctree(newVector, mass, 0, 0, 0, MAX_TREE_DIAMETER / 2);
        return retTree;
    }

    //Get the octant it needs to be in.
    int octant = getOctantVector(newVector, root->centerPosition);
    if(root->bodies[octant] == NULL) {
        //If that octant is open, just insert and return root.
        //Increase mass of overall tree, calculate new sub octree and insert it.
        float rad = root->dist / 2;
        float x = root->centerPosition.x + rad * (newVector.x > root->centerPosition.x ? 1 : -1);
        float y = root->centerPosition.y + rad * (newVector.y > root->centerPosition.y ? 1 : -1);
        float z = root->centerPosition.z + rad * (newVector.z > root->centerPosition.z ? 1 : -1);
        root->bodies[octant] = vectorToOctree(newVector, mass, x, y, z, rad);
        root->singleBody = 0;
    } else if(root->bodies[octant]->singleBody == 1) {
        //When there is already a single body there, subdivide the tree.
        root->bodies[octant] = subdivideOctree(root->bodies[octant], newVector, mass, 0);
    } else {
        //If octant is not a single body, and is not null, recursive call.
        root->bodies[octant] = insertElement(root->bodies[octant], newVector, mass);
    }
    Body retMass = combineMass(root->massPosition, root->mass, newVector, mass);
    root->massPosition = retMass.pos;
    root->mass = retMass.mass;
    return root;
}

Octree* vectorToOctree(Vec3f vector, float mass, float x, float y, float z, float dist) {
    //Generate new tree.
    Octree* newTree = (Octree*) malloc(sizeof(Octree));
    //We make the center of mass hold everything of the previous body.
    //Once it is no longer a single body, that data no longer matters.
    newTree->mass = mass;
    newTree->massPosition = vector;
    newTree->centerPosition.x = x;
    newTree->centerPosition.y = y;
    newTree->centerPosition.z = z;
    newTree->dist = dist;
    newTree->singleBody = 1;
    for(size_t i = 0; i < 8; i++) {
        newTree->bodies[i] = NULL;
    }
    //return the tree.
    return newTree;
}

Octree* subdivideOctree(Octree* rootTree, Vec3f newBody, float mass, int ct) {
    //Mark it as no longer a single body.
    rootTree->singleBody = 0;
    //Get the octant of the root and new body.
    int rootOctant = getOctantVector(rootTree->massPosition, rootTree->centerPosition);
    int newOctant = getOctantVector(newBody, rootTree->centerPosition);
    //printf("Octants are %d %d\n", rootOctant, newOctant);
    if(rootOctant == newOctant) {
        float distHalf = rootTree->dist / 2;
        float x = rootTree->centerPosition.x + (distHalf * (newBody.x > rootTree->centerPosition.x ? 1 : -1));
        float y = rootTree->centerPosition.y + (distHalf * (newBody.y > rootTree->centerPosition.y ? 1 : -1));
        float z = rootTree->centerPosition.z + (distHalf * (newBody.z > rootTree->centerPosition.z ? 1 : -1));
        rootTree->bodies[rootOctant] = subdivideOctree(vectorToOctree(rootTree->massPosition, rootTree->mass, x, y, z, distHalf), newBody, mass, ct + 1);
    } else {
        //Create sub-trees with the body that was center of mass, and the new body.
        rootTree->bodies[rootOctant] = vectorToOctree(rootTree->massPosition, rootTree->mass, rootTree->centerPosition.x, 
            rootTree->centerPosition.y, rootTree->centerPosition.z, rootTree->dist);
        rootTree->bodies[newOctant] = vectorToOctree(newBody, mass, rootTree->centerPosition.x, 
            rootTree->centerPosition.y, rootTree->centerPosition.z, rootTree->dist);
    }
    Body retBody = combineMass(rootTree->massPosition, rootTree->mass, newBody, mass);
    rootTree->massPosition = retBody.pos;
    rootTree->mass = retBody.mass;
    return rootTree;
}

int getOctantVector(Vec3f position, Vec3f centerPosition) {
    return getOctantPosition(position, centerPosition.x, centerPosition.y, centerPosition.z);
}

int getOctantPosition(Vec3f position, float x, float y, float z) {
    int ret = 0;
    if(position.x >= 0) {
        ret = position.y >= x ? 1 : 4;
    } else {
        ret = position.y >= y ? 2 : 3;
    }
    ret += position.z >= z ? 0 : 4;
    return ret - 1;
}

void _debugPrint(Octree* root, int leadingSpaces) {
    //This method looks gross, and it is, but it is only for debug display.
    if(root != NULL) {
        if(root->singleBody == 1) {
            for(int j = 0; j < leadingSpaces; j++) {
                printf(" ");
            }
            printf("Single Body Node Mass: %f, CMx: %f, CMy: %f, CMz: %f\n", root->mass, root->massPosition.x, root->massPosition.y, root->massPosition.z);
        } else {
            printf("--------------------------------------------------------------------------------\n");
            for(int j = 0; j < leadingSpaces; j++) {
                printf(" ");
            }
            printf("Subtree Mass: %f, CMx: %f, CMy: %f, CMz: %f\n", root->mass, root->massPosition.x, root->massPosition.y, root->massPosition.z);
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < leadingSpaces; j++) {
                    printf(" ");
                }
                printf("Printing child %d: \n", i);
                _debugPrint((root->bodies)[i], leadingSpaces + 3);
            }
            printf("--------------------------------------------------------------------------------\n\n");
        }
    } else {
        for(int j = 0; j < leadingSpaces; j++) {
            printf(" ");
        }
        printf("Tree or child is NULL!\n\n");
    }
}

void freeTree(Octree *root) {
    if(root == NULL) {
        return;
    }
    if(!root->singleBody) {
        for(size_t i = 0; i < 8; i++) {
            freeTree(root->bodies[i]);
        }
    }
    free(root);
}