#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"
#include "../common/octree.h"

int main(int argc, char **argv) {
    //This code is just some testing code. Ignore it.
    Octree* tree;
    Body a;
    a.mass = 5000.0;
    a.pos = newVec3f(-500.6,12.1,51.2);
    a.vel = newVec3f(12.6,643.2,12.5);
    tree = insertElement(tree, a);

    Body b;
    b.mass = 7000.0;
    b.pos = newVec3f(950.6,612.1,63.2);
    b.vel = newVec3f(12,51,21);
    tree = insertElement(tree, b);
    
    b.mass = 214.0;
    b.pos = newVec3f(512.6,-654.1,23.2);
    b.vel = newVec3f(12,34,67);
    tree = insertElement(tree, b);

    b.mass = 654.0;
    b.pos = newVec3f(-565.6,-742.1,215.2);
    b.vel = newVec3f(23,86,75);
    tree = insertElement(tree, b);

    b.mass = 581.0;
    b.pos = newVec3f(414.6,-421.1,412.2);
    b.vel = newVec3f(23,86,75);
    tree = insertElement(tree, b);

    b.mass = 232.0;
    b.pos = newVec3f(642.6,-521.1,767.2);
    b.vel = newVec3f(23,86,75);
    tree = insertElement(tree, b);

    _debugPrint(tree, 0);
}