#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vector_utils.h"
#include "octree.h"

Octree allocateOctreeCPU(int capacity) {
    Octree tree;
    tree.children = calloc(capacity * 8, sizeof(int));
    memset(tree.children, -1, capacity * 8 * sizeof(int));
    tree.centerPosition = calloc(capacity, sizeof(Vec3f));
    tree.massPosition = calloc(capacity, sizeof(Vec3f));
    tree.singleBody = calloc(capacity, sizeof(int));
    tree.mass = calloc(capacity, sizeof(float));
    tree.dist = calloc(capacity, sizeof(float));
    tree.nextIndex = calloc(1, sizeof(int));
    return tree;
}

void resetOctreeCPU(Octree tree, int capacity) {
    memset(tree.children, -1, capacity * 8 * sizeof(int));
    memset(tree.centerPosition, 0, capacity * sizeof(Vec3f));
    memset(tree.massPosition, 0, capacity * sizeof(Vec3f));
    memset(tree.singleBody, 0, capacity * sizeof(int));
    memset(tree.mass, 0, capacity * sizeof(float));
    memset(tree.dist, 0, capacity * sizeof(float));
    *tree.nextIndex = 0;
}

int insertElement(Octree root, int node, Vec3f newVector, float mass) {
    //If tree does not exist yet.
    if(*root.nextIndex == 0) {
        addOctreeChild(root, newVector, mass, 0, 0, 0, root.maxDiameter / 2);
        return node;
    }

    //Get the octant it needs to be in.
    int octant = getOctantVector(newVector, root.centerPosition[node]);
    if(root.children[node * 8 + octant] == -1) {
        //If that octant is open, just insert and return root.
        //Increase mass of overall tree, calculate new sub octree and insert it.
        float rad = root.dist[node] / 2;
        float x = root.centerPosition[node].x + rad * (newVector.x > root.centerPosition[node].x ? 1 : -1);
        float y = root.centerPosition[node].y + rad * (newVector.y > root.centerPosition[node].y ? 1 : -1);
        float z = root.centerPosition[node].z + rad * (newVector.z > root.centerPosition[node].z ? 1 : -1);
        root.children[node * 8 + octant] = addOctreeChild(root, newVector, mass, x, y, z, rad);
        root.singleBody[node] = 0;
    } else if(root.singleBody[root.children[node * 8 + octant]] == 1) {
        //When there is already a single body there, subdivide the tree.
        subdivideOctree(root, root.children[node * 8 + octant], newVector, mass);
    } else {
        //If octant is not a single body, and is not null, recursive call.
        insertElement(root, root.children[node * 8 + octant], newVector, mass);
    }
    Body retMass = combineMass(root.massPosition[node], root.mass[node], newVector, mass);
    root.massPosition[node] = retMass.pos;
    root.mass[node] = retMass.mass;
    return node;
}

int addOctreeChild(Octree tree, Vec3f vector, float mass, float x, float y, float z, float dist) {
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

int subdivideOctree(Octree root, int node, Vec3f newBody, float mass) {
    int flag = 1;
    int rootOctant = 0;
    int newOctant = 0;
    int useThisToReturn = node;
    do {
        flag = 0;
        //Mark it as no longer a single body.
        root.singleBody[node] = 0;
        //Get the octant of the root and new body.
        rootOctant = getOctantVector(root.massPosition[node], root.centerPosition[node]);
        newOctant = getOctantVector(newBody, root.centerPosition[node]);
        if(rootOctant != newOctant) {
            root.children[node * 8 + rootOctant] = addOctreeChild(root, root.massPosition[node], root.mass[node],
              root.centerPosition[node].x, root.centerPosition[node].y, root.centerPosition[node].z, root.dist[node]);
            root.children[node * 8 + newOctant] = addOctreeChild(root, newBody, mass, root.centerPosition[node].x,
                 root.centerPosition[node].y, root.centerPosition[node].z, root.dist[node]);
            flag = 0;
        } else {
            float distHalf = root.dist[node] / 2;
            float x = root.centerPosition[node].x + (distHalf * (newBody.x > root.centerPosition[node].x ? 1 : -1));
            float y = root.centerPosition[node].y + (distHalf * (newBody.y > root.centerPosition[node].y ? 1 : -1));
            float z = root.centerPosition[node].z + (distHalf * (newBody.z > root.centerPosition[node].z ? 1 : -1));
            int newChild = addOctreeChild(root, root.massPosition[node], root.mass[node], x, y, z, distHalf);
            root.children[node * 8 + rootOctant] = newChild;
            node = newChild;
        }
        Body retBody = combineMass(root.massPosition[node], root.mass[node], newBody, mass);
        root.massPosition[node] = retBody.pos;
        root.mass[node] = retBody.mass;
    } while(flag);

    return useThisToReturn;
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

void freeTreeCPU(Octree root) {
    free(root.children);
    free(root.centerPosition);
    free(root.massPosition);
    free(root.singleBody);
    free(root.mass);
    free(root.dist);
    free(root.nextIndex);
}

Octree setDiameter(Octree tree, float maxSize) {
    tree.maxDiameter = maxSize;
    return tree;
}


void _debugPrint(Octree root, int node, int leadingSpaces) {
    //This method looks gross, and it is, but it is only for debug display.
    if(node != -1) {
        if(root.singleBody[node] == 1) {
            for(int j = 0; j < leadingSpaces; j++) {
                printf(" ");
            }
            printf("Single Body Node Mass: %f, CMx: %f, CMy: %f, CMz: %f\n", root.mass[node], root.massPosition[node].x, root.massPosition[node].y, root.massPosition[node].z);
        } else {
            printf("--------------------------------------------------------------------------------\n");
            for(int j = 0; j < leadingSpaces; j++) {
                printf(" ");
            }
            printf("Subtree Mass: %f, CMx: %f, CMy: %f, CMz: %f\n", root.mass[node], root.massPosition[node].x, root.massPosition[node].y, root.massPosition[node].z);
            for(int i = 0; i < 8; i++) {
                for(int j = 0; j < leadingSpaces; j++) {
                    printf(" ");
                }
                printf("Printing child %d: \n", i);
                _debugPrint(root, root.children[node * 8 + i], leadingSpaces + 3);
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