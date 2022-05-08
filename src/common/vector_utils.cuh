#pragma once

#include "vector_utils.h"

/**
 * @brief Creates a new vector with the given x,y,z parameters and stores it in out
 * 
 * @param x 
 * @param y 
 * @param z 
 * @param out 
 */
__device__ Vec3f newVec3fGPU(float x, float y, float z);

/**
 * @brief Adds two vectors and stores the result in u
 * 
 * @param u 
 * @param v 
 */
__device__ Vec3f vectorAddGPU(Vec3f u, Vec3f v);

/**
 * @brief Scales u by t
 * 
 * @param t 
 * @param u 
 */
__device__ Vec3f vectorScalarMultGPU(float t, Vec3f u);

/**
 * @brief Computes the vector dot product of u and v
 * 
 * @param u 
 * @param v 
 * @return float 
 */
__device__ float vectorDotGPU(Vec3f u, Vec3f v);

/**
 * @brief Computes the distance betwwen two points
 * 
 * @param u Point 1
 * @param v Point 2
 * @return float 
 */
__device__ float distanceBetweenPointsGPU(Vec3f u, Vec3f v);

/**
 * @brief Converts vector u to a unit vector
 * 
 * @param u 
 */
__device__ Vec3f vectorNormalizeGPU(Vec3f u);

__device__ int vectorEqGPU(Vec3f u, Vec3f v);

/**
 * @brief Constructs a vector starting at p1 pointing towards p2
 * 
 * @param p1 
 * @param p2 
 * @return double* 
 */
__device__ Vec3f ptToVectorGPU(Vec3f p1, Vec3f p2);

__device__ Vec3f finalVelGPU(Vec3f accel, Vec3f v0, double t);

__device__ Vec3f finalPosGPU(Vec3f accel, Vec3f v0, Vec3f p0, double t);

/**
 * @brief Combines the mass of two bodies.
 * 
 * @param rootVector The root center of mass.
 * @param rootMass The root mass.
 * @param newVector The new body center of mass.
 * @param newMass The new body mass.
 * 
 * @return Body A body of the combined masses.
 */
__device__ Body combineMassGPU(Vec3f rootVector, float rootMass, Vec3f newVector, float newMass);

/**
 * @brief Creates a new empty body.
 * 
 */
__device__ Body newEmptyBodyGPU();