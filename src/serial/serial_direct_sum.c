#include <stdlib.h>
#include <stdio.h>
#include "../common/VectorUtils.h"
#include "../common/fileIO_util.h"

#define G 1//6.67430e-11




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
        printf("Usage: <path to input> <number of inputs> <number of timesteps> <length of time steps in seconds>\n");
        exit(0);
    }

    filePath = argv[1];
    bodyCount = strtoul(argv[2], NULL, 10);
    timeSteps = strtoul(argv[3], NULL, 10);
    deltaT = atof(argv[4]);
    
    data = fopen(filePath, "r");

    frames = (Body **) malloc(timeSteps * sizeof(Body *));

    for(size_t i = 0; i<timeSteps; i++)
    {
        frames[i] = (Body *) malloc(bodyCount * sizeof(Body));
    }

    readInput(data, frames[0], bodyCount);

    fclose(data);

    for(size_t i = 0; i<timeSteps - 1; i++)
    {
        float t = i * deltaT;
        for(size_t j = 0; j<bodyCount; j++)
        {
            Vec3f netForce = newVec3f(0,0,0);
            for(size_t k = 0; k<bodyCount; k++)
            {
                if (j == k) continue;

                Vec3f dist = ptToVector(frames[i][j].pos, frames[i][k].pos);
                double forceMag = G * (frames[i][j].mass * frames[i][k].mass) / vectorDot(dist, dist);
                Vec3f dir = vectorNormalize(dist);
                Vec3f force = vectorScalarMult(forceMag, dir);
                netForce = vectorAdd(netForce, dist);
            }
            Vec3f accel = vectorScalarMult(1/frames[i][j].mass, netForce);
            frames[i+1][j].mass = frames[i][j].mass;
            frames[i+1][j].pos = finalPos(netForce, frames[i][j].vel, frames[i][j].pos, t);
            frames[i+1][j].vel = finalVel(netForce, frames[i][j].vel, t);
        }
    }
    float *masses;
    masses = (float *)malloc(bodyCount * sizeof(float));
    Vec3f **pos;
    pos = (Vec3f **)malloc(timeSteps * sizeof(Vec3f *));
    for(size_t i = 0; i< timeSteps; i++)
    {
        pos[i] = (Vec3f *)malloc(bodyCount * sizeof(Vec3f));
    }

    for(size_t i = 0; i<bodyCount; i++)
    {
        masses[i] = frames[0][i].mass;
    }

    for(size_t i = 0; i<timeSteps; i++)
    {
        for(size_t j = 0; j<bodyCount; j++)
        {
            pos[i][j] = frames[i][j].pos;
        }
    }

    out = fopen("out.nbody", "wb");
    writeOutput(out, bodyCount, timeSteps, masses, pos);
    fclose(out);

    for(size_t i = 0; i < timeSteps; i++)
    {
        free(frames[i]);
        free(pos[i]);
    }

    free(frames);
    free(pos);
    free(masses);

}