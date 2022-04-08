#include "VectorUtils.h"
#include <math.h>

void newVec3f(float x, float y, float z, Vec3f *out)
{
    out->x = x;
    out->y = y;
    out->z = z;
}

void vectorAdd(Vec3f *u, Vec3f *v)
{
    u->x += v->x;
    u->y += v->y;
    u->z += v->z;
}

void vectorScalarMult(float t, Vec3f *u)
{
    u->x *= t;
    u->y *= t;
    u->z *= t;
}

float vectorDot(Vec3f *u, Vec3f *v)
{
    return u->x * v->x + u->y * v->y + u->z * v->z;
}

void vectorNormalize(Vec3f *u)
{
    float normalizationFactor = 1/sqrt(vectorDot(u, u));

    vectorScalarMult(normalizationFactor, u);
}

void ptToVector(Vec3f *p1, Vec3f *p2, Vec3f *out)
{
    newVec3f(p2->x - p1->x, p2->y - p1->y, p2->z - p1->z, out);
}