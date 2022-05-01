#include <stdlib.h>
#include <stdio.h>
#include "vector_utils.h"
#include "octree.h"

#define MAX_TREE_DIAMETER 100000

Octree* insertElement(Octree* root, Body newBody) {
    //If tree does not exist.
    if(root == NULL) {
        //Create tree and set all values to be defaults.
        Body emptyBody = newEmptyBody();
        Octree* retTree;
        float radHalf = MAX_TREE_DIAMETER / 2;
        retTree = bodyToOctree(emptyBody, 0, 0, 0, radHalf);
        retTree->rad = radHalf;
        root = retTree;
    } 

    //Get the octant it needs to be in.
    int octant = getOctantVector(newBody, root->centerPosition);
    if(root->bodies[octant] == NULL) {
        //If that octant is open, just insert and return root.
        //Increase mass of overall tree, calculate new sub octree and insert it.
        float radHalf = root->rad / 2;
        float x = root->centerPosition.x + radHalf * (newBody.pos.x > root->centerPosition.x ? 1 : -1);
        float y = root->centerPosition.y + radHalf * (newBody.pos.y > root->centerPosition.y ? 1 : -1);
        float z = root->centerPosition.z + radHalf * (newBody.pos.z > root->centerPosition.z ? 1 : -1);
        root->bodies[octant] = bodyToOctree(newBody, x, y, z, radHalf);
        root->singleBody = 0;
    } else if(root->bodies[octant]->singleBody == 1) {
        //When there is already a single body there, subdivide the tree.
        root->bodies[octant] = subdivideOctree(root->bodies[octant], newBody);
    } else {
        //If octant is not a single body, and is not null, recursive call.
        root->bodies[octant] = insertElement(root->bodies[octant], newBody);
    }
    root->centerOfMass = combineMass(root->centerOfMass, newBody);
    return root;
}

Octree* bodyToOctree(Body body, float x, float y, float z, float rad) {
    //Generate new tree.
    Octree* newTree = (Octree*) malloc(sizeof(Octree));
    //We make the center of mass hold everything of the previous body.
    //Once it is no longer a single body, that data no longer matters.
    newTree->centerOfMass.mass = body.mass;
    newTree->centerOfMass.pos = body.pos;
    newTree->centerOfMass.vel = body.vel;
    newTree->centerPosition.x = x;
    newTree->centerPosition.y = y;
    newTree->centerPosition.z = z;
    newTree->rad = rad;
    newTree->singleBody = 1;
    //return the tree.
    return newTree;
}

Octree* subdivideOctree(Octree* rootTree, Body newBody) {
    //Mark it as no longer a single body.
    rootTree->singleBody = 0;
    //Get the octant of the root and new body.
    int rootOctant = getOctantVector(rootTree->centerOfMass, rootTree->centerPosition);
    int newOctant = getOctantVector(newBody, rootTree->centerPosition);
    if(rootOctant == newOctant) {
        float radHalf = rootTree->rad / 2;
        float x = rootTree->centerPosition.x + radHalf * (newBody.pos.x > rootTree->centerPosition.x ? 1 : -1);
        float y = rootTree->centerPosition.y + radHalf * (newBody.pos.y > rootTree->centerPosition.y ? 1 : -1);
        float z = rootTree->centerPosition.z + radHalf * (newBody.pos.z > rootTree->centerPosition.z ? 1 : -1);
        rootTree->bodies[rootOctant] = subdivideOctree(bodyToOctree(rootTree->centerOfMass, x, y, z, radHalf), newBody);
    } else {
        //Create sub-trees with the body that was center of mass, and the new body.
        rootTree->bodies[rootOctant] = bodyToOctree(rootTree->centerOfMass, rootTree->centerPosition.x, 
            rootTree->centerPosition.y, rootTree->centerPosition.z, rootTree->rad);
        rootTree->bodies[newOctant] = bodyToOctree(newBody, rootTree->centerPosition.x, 
            rootTree->centerPosition.y, rootTree->centerPosition.z, rootTree->rad);
    }
    rootTree->centerOfMass = combineMass(rootTree->centerOfMass, newBody);
    return rootTree;
}

int getOctantVector(Body body, Vec3f centerPosition) {
    return getOctantPosition(body, centerPosition.x, centerPosition.y, centerPosition.z);
}

int getOctantPosition(Body body, int x, int y, int z) {
    int ret = 0;
    if(body.pos.x >= 0) {
        ret = body.pos.y >= x ? 1 : 4;
    } else {
        ret = body.pos.y >= y ? 2 : 3;
    }
    ret += body.pos.z >= z ? 0 : 4;
    return ret - 1;
}

void _debugPrint(Octree* root, int leadingSpaces) {
    //This method looks gross, and it is, but it is only for debug display.
    if(root != NULL) {
        if(root->singleBody == 1) {
            for(int j = 0; j < leadingSpaces; j++) {
                printf(" ");
            }
            printf("Single Body Node Mass: %f, CMx: %f, CMy: %f, CMz: %f\n", root->centerOfMass.mass, root->centerOfMass.pos.x, root->centerOfMass.pos.y, root->centerOfMass.pos.z);
        } else {
            printf("--------------------------------------------------------------------------------\n");
            for(int j = 0; j < leadingSpaces; j++) {
                printf(" ");
            }
            printf("Subtree Mass: %f, CMx: %f, CMy: %f, CMz: %f\n", root->centerOfMass.mass, root->centerOfMass.pos.x, root->centerOfMass.pos.y, root->centerOfMass.pos.z);
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