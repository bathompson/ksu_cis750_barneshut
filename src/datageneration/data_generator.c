#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"

float createRandom(float basisPoint, float scaleFactor);

#define MAX_MASS 1000000000;

/**
 * @brief Generates data for bodies.
 * 
 * Generates for each body i:
 * Mass Kg
 * Posx, Posy, Posz m
 * Velx, Vely, Velz m/s
 * 
 * @param argc 
 * @param argv 
 * @return int Exit code. 
 */
void main(int argc, char **argv) {
    //Declarations
    float globalSize;
    float clusterDistRatio;
    float percentBodiesInClusters;
    float scaleFactor;
    size_t numBodies;
    size_t numClusters;
    size_t numFreeBodies;
    size_t numBodiesInClusters;
    size_t yeet;
    Body *bodies;
    
    //Check Arguments
    if(argc != 6) {
        printf("Invalid input format. Format via: globalSize[0,sizeof(float)], ");
        printf("numBodies[n], numClusters[n], percentBodiesInClusters[0,1], clusterDistRatio[0,1]\n");
        printf("\nThe Program has exited with code -1.\n");
        return;
    }

    //Convert input data.
    globalSize = atof(argv[1]);
    numBodies = strtoul(argv[2], NULL, 10);
    numClusters = strtoul(argv[3], NULL, 10);
    percentBodiesInClusters = atof(argv[4]);
    clusterDistRatio = atof(argv[5]);

    //Allocate memory, do some setup math, seed random.
    bodies = (Body*) (malloc(sizeof(Body) * numBodies));
    numBodiesInClusters = (size_t) (percentBodiesInClusters * numBodies);
    numFreeBodies = numBodies - numBodiesInClusters;
    srand(time(NULL));

    //Calculate free bodies
    for(yeet = 0; yeet < numFreeBodies; yeet++) {
        Vec3f posVector;
        Vec3f velVector;
        Body b;
        posVector.x = createRandom(0, globalSize);
        posVector.y = createRandom(0, globalSize);
        posVector.z = createRandom(0, globalSize);
        //Max velocity in any direction is 10% of scale factor.
        velVector.x = createRandom(0, globalSize * .1);
        velVector.y = createRandom(0, globalSize * .1);
        velVector.z = createRandom(0, globalSize * .1);
        b.mass = ((float) rand() / (float) (RAND_MAX)) * MAX_MASS;
        b.pos = posVector;
        b.vel = velVector;

        bodies[yeet] = b;
    }
    
    //Calculate clusters
    /*
    for(int i = 0; i < numClusters; i++) {
        float xBasis = createRandom(0, globalSize);
        float yBasis = createRandom(0, globalSize);
        float zBasis = createRandom(0, globalSize);
        scaleFactor = globalSize * clusterDistRatio;
        size_t bodiesInGivenCluster = (numBodiesInClusters / numClusters) + (numBodiesInClusters % numClusters < i ? 0 : 1);
        for(int j = 0; j < bodiesInGivenCluster; j++) {
            Vec3f posVector;
            Vec3f velVector;
            Body b;
            posVector.x = createRandom(xBasis, scaleFactor);
            posVector.y = createRandom(yBasis, scaleFactor);
            posVector.z = createRandom(zBasis, scaleFactor);
            //Max velocity in any direction is 10% of scale factor.
            velVector.x = createRandom(0, scaleFactor * .1);
            velVector.y = createRandom(0, scaleFactor * .1);
            velVector.z = createRandom(0, scaleFactor * .1);
            b.mass = ((float) rand() / (float) (RAND_MAX)) * MAX_MASS;
            b.pos = posVector;
            b.vel = velVector;
            
            bodies[yeet] = b;
            yeet++;
        }
    }
    */
    //Export as a text file.
    writeBodies("outputFile.csv", bodies, numBodies);

    //Free memory
    free(bodies);
}

/**
 * @brief Create a random float.
 * 
 * @param basisPoint The center point to base the values around.
 * @param scaleFactor The range of the values from around the basis point.
 * @return float The random float.
 */
float createRandom(float basisPoint, float scaleFactor) {
    float ret = (basisPoint + ((float) rand() / (float) (RAND_MAX))) * scaleFactor;
    ret *= ((rand() % 2) * -1);
    return ret;
}