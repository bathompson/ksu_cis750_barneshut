#pragma once
typedef struct {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct {
    Vec3f pos;
    Vec3f vel;
    float mass;
} Body;

/**
 * @brief Creates a new vector with the given x,y,z parameters and stores it in out
 * 
 * @param x 
 * @param y 
 * @param z 
 * @param out 
 */
Vec3f newVec3f(float x, float y, float z);

/**
 * @brief Adds two vectors and stores the result in u
 * 
 * @param u 
 * @param v 
 */
Vec3f vectorAdd(Vec3f u, Vec3f v);

/**
 * @brief Scales u by t
 * 
 * @param t 
 * @param u 
 */
Vec3f vectorScalarMult(float t, Vec3f u);

/**
 * @brief Computes the vector dot product of u and v
 * 
 * @param u 
 * @param v 
 * @return float 
 */
float vectorDot(Vec3f u, Vec3f v);

/**
 * @brief Computes the distance betwwen two points
 * 
 * @param u Point 1
 * @param v Point 2
 * @return float 
 */
float distanceBetweenPoints(Vec3f u, Vec3f v);

/**
 * @brief Converts vector u to a unit vector
 * 
 * @param u 
 */
Vec3f vectorNormalize(Vec3f u);

int vectorEq(Vec3f u, Vec3f v);

/**
 * @brief Constructs a vector starting at p1 pointing towards p2
 * 
 * @param p1 
 * @param p2 
 * @return double* 
 */
Vec3f ptToVector(Vec3f p1, Vec3f p2);

Vec3f finalVel(Vec3f accel, Vec3f v0, double t);

Vec3f finalPos(Vec3f accel, Vec3f v0, Vec3f p0, double t);

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
Body combineMass(Vec3f rootVector, float rootMass, Vec3f newVector, float newMass);

/**
 * @brief Creates a new empty body.
 * 
 */
Body newEmptyBody();