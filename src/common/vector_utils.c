#include <math.h>
#include <stdio.h>
#include "vector_utils.h"

Vec3f newVec3f(float x, float y, float z) {
    Vec3f ret;
    ret.x= x;
    ret.y = y;
    ret.z = z;

    return ret;
}

Vec3f vectorAdd(Vec3f u, Vec3f v) {
    return newVec3f(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vec3f vectorScalarMult(float t, Vec3f u) {
    return newVec3f(t*u.x, t*u.y, t*u.z);
}

Vec3f vectorNormalize(Vec3f u) {
    float normalizationFactor = 1/sqrtf(vectorDot(u, u));
    return vectorScalarMult(normalizationFactor, u);
}

Vec3f ptToVector(Vec3f p1, Vec3f p2) {
    return newVec3f(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
}

Vec3f finalVel(Vec3f accel, Vec3f v0, double t) {
    return newVec3f(accel.x * t + v0.x, accel.y * t + v0.y, accel.y * t + v0.y);
}

Vec3f finalPos(Vec3f accel, Vec3f v0, Vec3f p0, double t) {
    return newVec3f(1/2 * accel.x * t * t + v0.x * t + p0.x, 
                    1/2 * accel.y * t * t + v0.y * t + p0.y,
                    1/2 * accel.z * t * t + v0.z * t + p0.z);
}

float vectorDot(Vec3f u, Vec3f v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

float distanceBetweenPoints(Vec3f u, Vec3f v) {
    return sqrtf(u.x * v.x + u.y * v.y + u.z * v.z);

}

int vectorEq(Vec3f u, Vec3f v) {
    return u.x == v.x && u.y == v.y && u.z == v.z;
}

Body combineMass(Vec3f rootVector, float rootMass, Vec3f newVector, float newMass) {
    //Total mass is sum
    //CM = (x1m1 + x2m2)/(m1 + m2)
    Body combinedMass;
    combinedMass.mass = rootMass + newMass;
    combinedMass.pos.x = ((rootMass * rootVector.x) + (newMass * newVector.x)) / (rootMass + newMass);
    combinedMass.pos.y = ((rootMass * rootVector.y) + (newMass * newVector.y)) / (rootMass + newMass);
    combinedMass.pos.z = ((rootMass * rootVector.z) + (newMass * newVector.z)) / (rootMass + newMass);
    return combinedMass;
}

Body newEmptyBody() {
    Body b;
    b.mass = 0;
    b.pos = newVec3f(0, 0, 0);
    b.vel = newVec3f(0, 0, 0);
    return b;
}