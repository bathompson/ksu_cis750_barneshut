#pragma once
typedef struct
{
    float x;
    float y;
    float z;
} Vec3f;

/**
 * @brief Creates a new vector with the given x,y,z parameters and stores it in out
 * 
 * @param x 
 * @param y 
 * @param z 
 * @param out 
 */
void newVec3f(float x, float y, float z, Vec3f *out);

/**
 * @brief Adds two vectors and stores the result in u
 * 
 * @param u 
 * @param v 
 */
void vectorAdd(Vec3f *u, Vec3f *v);

/**
 * @brief Scales u by t
 * 
 * @param t 
 * @param u 
 */
void vectorScalarMult(float t, Vec3f *u);

/**
 * @brief Computes the vector dot product of u and v
 * 
 * @param u 
 * @param v 
 * @return float 
 */
float vectorDot(Vec3f *u, Vec3f *v);

/**
 * @brief Converts vector u to a unit vector
 * 
 * @param u 
 */
void vectorNormalize(Vec3f *u);

/**
 * @brief Constructs a vector starting at p1 pointing towards p2
 * 
 * @param p1 
 * @param p2 
 * @return double* 
 */
void ptToVector(Vec3f *p1, Vec3f *p2, Vec3f *out);