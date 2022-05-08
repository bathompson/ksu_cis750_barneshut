#include "vector_utils.cuh"

__device__ Vec3f newVec3fGPU(float x, float y, float z)
{
    Vec3f ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    return ret;
}

__device__ float vectorDotGPU(Vec3f u, Vec3f v)
{
    return u.x*v.x + u.y*v.y + u.z*v.z;
}

__device__ Vec3f vectorAddGPU(Vec3f u, Vec3f v)
{
    return newVec3fGPU(u.x + v.x, u.y + v.y, u.z + v.z);
}

__device__ Vec3f vectorScalarMultGPU(float t, Vec3f u)
{
    return newVec3fGPU(t*u.x, t*u.y, t*u.z);
}

__device__ Vec3f vectorNormalizeGPU(Vec3f u)
{
    return vectorScalarMultGPU(rnorm3df(u.x, u.y, u.z), u);
}

__device__ int vectorEqGPU(Vec3f u, Vec3f v) {
    return u.x == v.x && u.y == v.y && u.z == v.z;
}

__device__ Vec3f ptToVectorGPU(Vec3f p1, Vec3f p2)
{
    return newVec3fGPU(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
}

__device__ Vec3f finalVelGPU(Vec3f accel, Vec3f v0, double t)
{
    return newVec3fGPU(accel.x * t + v0.x, accel.y * t + v0.y, accel.y * t + v0.y);
}

__device__ Vec3f finalPosGPU(Vec3f accel, Vec3f v0, Vec3f p0, double t)
{
    return newVec3fGPU(fdividef(accel.x*t*t,2) + v0.x*t + p0.x,
                    fdividef(accel.y*t*t,2) + v0.y*t + p0.y,
                    fdividef(accel.z*t*t,2) + v0.z*t + p0.z);
}

__device__ float distanceBetweenPointsGPU(Vec3f u, Vec3f v) {
    return sqrt(u.x * v.x + u.y * v.y + u.z * v.z);

}

__device__ Body combineMassGPU(Vec3f rootVector, float rootMass, Vec3f newVector, float newMass) {
    //Total mass is sum
    //CM = (x1m1 + x2m2)/(m1 + m2)
    Body combinedMass;
    combinedMass.mass = rootMass + newMass;
    combinedMass.pos.x = ((rootMass * rootVector.x) + (newMass * newVector.x)) / (rootMass + newMass);
    combinedMass.pos.y = ((rootMass * rootVector.y) + (newMass * newVector.y)) / (rootMass + newMass);
    combinedMass.pos.z = ((rootMass * rootVector.z) + (newMass * newVector.z)) / (rootMass + newMass);
    return combinedMass;
}

__device__ Body newEmptyBodyGPU() {
    Body b;
    b.mass = 0;
    b.pos = newVec3fGPU(0, 0, 0);
    b.vel = newVec3fGPU(0, 0, 0);
    return b;
}