#include <stdlib.h>
#include <stdio.h>
#include "VectorUtils.h"

#define G 6.67430e-11

typedef struct
{
    Vec3f pos;
    Vec3f vel;
    float mass;
} Body;

Vec3f finalVel(Vec3f *accel, Vec3f *v0, double t)
{
    Vec3f ret;
    ret.x = accel->x * t + v0->x;
    ret.y = accel->y * t + v0->y;
    ret.y = accel->y * t + v0->y;
    return ret;
}

Vec3f finalPos(Vec3f *accel, Vec3f *v0, Vec3f *p0, double t)
{
    Vec3f ret;
    ret.x = 1/2 * accel->x * t * t + v0->x * t + p0->x;
    ret.y = 1/2 * accel->y * t * t + v0->y * t + p0->y;
    ret.z = 1/2 * accel->z * t * t + v0->z * t + p0->z;
    return ret;
}

int main(int argc, char **argv)
{
    FILE *data;
    char *filePath;
    FILE *out;
    Body **frames;
    size_t bodyCount;
    size_t timeSteps;
    float deltaT;

    if(argc < 5)
    {
        printf("Usage: <path to input> <number of inputs> <number of timesteps> <length of time steps in seconds>");
        exit(0);
    }

    filePath = argv[1];
    bodyCount = strtoul(argv[2], NULL, 10);
    timeSteps = strtoul(argv[3], NULL, 10);
    deltaT = atof(argv[4]);
    

    frames = (Body **) malloc(timeSteps * sizeof(Body *));

    for(size_t i = 0; i<timeSteps; i++)
    {
        frames[i] = (Body *) malloc(timeSteps * sizeof(Body));
    }

    char *line;
    for(size_t i = 0; i<bodyCount; i++)
    {
        
    }

    for(size_t i = 0; i<timeSteps - 1; i++)
    {
        float t = i * deltaT;
        for(size_t j = 0; j<bodyCount; j++)
        {
            Vec3f netForce;
            newVec3f(0,0,0, &netForce);
            for(size_t k = 0; k<bodyCount; k++)
            {
                if (j == k) continue;

                Vec3f dist;
                ptToVector(&frames[i][j].pos, &frames[i][k].pos, &dist);
                double force = G * (frames[i][j].mass * frames[i][k].mass) / vectorDot(&dist, &dist);
                vectorNormalize(&dist);
                //dist is now a unit vector
                vectorScalarMult(force, &dist);
                vectorAdd(&netForce, &dist);
            }
            vectorScalarMult(1/frames[i][j].mass, &netForce);
            //netForce is now acceleration
            frames[i+1][j].mass = frames[i][j].mass;
            frames[i+1][j].pos = finalPos(&netForce, &frames[i][j].vel, &frames[i][j].pos, t);
            frames[i+1][j].vel = finalVel(&netForce, &frames[i][j].vel, t);
        }
    }
    float masses[bodyCount];
    Vec3f pos[timeSteps][bodyCount];
    for(size_t i = 0; i<bodyCount; i++)
    {
        masses[i] = frames[0][i].mass;
    }

    for(size_t i = 0; i<timeSteps; i++)
    {
        for(size_t j = 0; j<timeSteps; j++)
        {
            pos[i][j] = frames[i][j].pos;
        }
    }

    out = fopen("out.nbody", "wb");
    fwrite(&bodyCount, sizeof(size_t), 1, out);
    fwrite(masses, sizeof(float), bodyCount, out);
    for(size_t i = 0; i<timeSteps; i++)
    {
        fwrite(pos[i], sizeof(Vec3f), bodyCount, out);
    }
    fclose(out);

    for(size_t i = 0; i < timeSteps; i++)
    {
        free(frames[i]);
    }

    free(frames);

}