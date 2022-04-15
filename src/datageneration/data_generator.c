#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"

/**
 * @brief Create a random float.
 * 
 * @param basisPoint The center point to base the values around.
 * @param scaleFactor The range of the values from around the basis point.
 * @param enableInversion Enable inverting the polarity.
 * @return float The random float.
 */
float createRandom(float basisPoint, float scaleFactor, int enableInversion);
//Defined up here to save some passing.
float globalSize;

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
int main(int argc, char **argv) {
    //Declarations
    float clusterDistRatio;
    float percentBodiesInClusters;
    float scaleFactor;
    float maximumMass;
    float velocityMultipler;
    size_t numBodies;
    size_t numClusters;
    size_t numFreeBodies;
    size_t numBodiesInClusters;
    size_t constantIndex;
    Body *bodies;
    
    //Check Arguments
    //Command args I have been using:
    //100000000000 10000 15 .8 .05 500000 .00005
    if(argc != 8) {
        printf("Invalid input format. Format via: globalSize[0,MAX_FLOAT], ");
        printf("numBodies[n], numClusters[n], percentBodiesInClusters[0,1], clusterDistRatio[0,1], ");
        printf("maximumMass[0,MAX_FLOAT], velocityMultiplier[0,1]\n");
        return -1;
    }

    //Convert input data.
    globalSize = atof(argv[1]);
    numBodies = strtoul(argv[2], NULL, 10); //If you use less than 4085 you get a corrupted top size.
    numClusters = strtoul(argv[3], NULL, 10);
    percentBodiesInClusters = atof(argv[4]);
    clusterDistRatio = atof(argv[5]);
    maximumMass = atof(argv[6]);
    velocityMultipler = atof(argv[7]);
    scaleFactor = globalSize * clusterDistRatio;

    //Allocate memory, do some setup math, seed random.
    bodies = (Body*) (malloc(sizeof(Body) * numBodies));
    numBodiesInClusters = (size_t) (percentBodiesInClusters * numBodies);
    numFreeBodies = numBodies - numBodiesInClusters;
    srand(time(NULL));

    //Calculate free bodies
    for(constantIndex = 0; constantIndex < numFreeBodies; constantIndex++) {
        Vec3f posVector;
        Vec3f velVector;
        Body b;
        posVector.x = createRandom(0, globalSize, 1);
        posVector.y = createRandom(0, globalSize, 1);
        posVector.z = createRandom(0, globalSize, 1);
        velVector.x = createRandom(0, globalSize * velocityMultipler, 1);
        velVector.y = createRandom(0, globalSize * velocityMultipler, 1);
        velVector.z = createRandom(0, globalSize * velocityMultipler, 1);
        b.mass = ((float) rand() / (float) (RAND_MAX)) * maximumMass;
        b.pos = posVector;
        b.vel = velVector;

        bodies[constantIndex] = b;
    }
    
    //Calculate clusters
    for(int i = 0; i < numClusters; i++) {
        float xBasis = createRandom(0, globalSize, 1);
        float yBasis = createRandom(0, globalSize, 1);
        float zBasis = createRandom(0, globalSize, 1);
        for(int j = 0; j < (numBodiesInClusters / numClusters) + (numBodiesInClusters % numClusters < i ? 0 : 1); j++) {
            Vec3f posVector;
            Vec3f velVector;
            Body b;
            posVector.x = createRandom(xBasis, scaleFactor, 0);
            posVector.y = createRandom(yBasis, scaleFactor, 0);
            posVector.z = createRandom(zBasis, scaleFactor, 0);
            //Max velocity in any direction is 10% of scale factor.
            velVector.x = createRandom(0, scaleFactor * velocityMultipler, 0);
            velVector.y = createRandom(0, scaleFactor * velocityMultipler, 0);
            velVector.z = createRandom(0, scaleFactor * velocityMultipler, 0);
            b.mass = ((float) rand() / (float) (RAND_MAX)) * maximumMass;
            b.pos = posVector;
            b.vel = velVector;
            
            bodies[constantIndex] = b;
            constantIndex++;
        }
    }
    
    //Export as a text file.
    writeBodies("outputFile.csv", bodies, numBodies);

    //Free memory
    free(bodies);
    return 0;
}

float createRandom(float basisPoint, float scaleFactor, int enableInversion) {
    float ret; 
    do {
        ret = basisPoint + (((float) rand() / (float) (RAND_MAX)) * scaleFactor);
    } while(abs(ret) > globalSize);
    ret *= enableInversion ? ((rand() % 2) ? -1 : 1) : 1;
    return ret;
}