#include <stdlib.h>
#include <stdio.h>
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"
#include "../common/octree.h"

int main(int argc, char **argv) {
    Octree* tree;
    Body a;
    a.mass = 5000.0;
    a.pos = newVec3f(-500.6,12.1,51.2);
    a.vel = newVec3f(12.6,643.2,12.5);
    tree = insertElement(tree, a);

    Body b;
    b.mass = 7000.0;
    b.pos = newVec3f(1240.6,612.1,63.2);
    b.vel = newVec3f(12,51,21);
    tree = insertElement(tree, b);
    
    b.mass = 214.0;
    b.pos = newVec3f(512.6,-654.1,23.2);
    b.vel = newVec3f(12,5231,67);
    tree = insertElement(tree, b);

    _debugPrint(tree, 0);
}