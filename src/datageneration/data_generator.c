#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../common/vector_utils.h"
#include "../common/fileIO_util.h"

/**
 * @brief The types of data shapes to generate.
 * 
 */
enum ShapeType {Cube, Sphere};

/**
 * @brief Create a random float in the shape of a cube.
 * 
 * @param basisPoint The center point to base the values around.
 * @param scaleFactor The range of the values from around the basis point.
 * @param enableInversion Enable inverting the polarity.
 * @param type The type of shape we are creating.
 * @return float The random float.
 */
float createRandom(float basisPoint, float scaleFactor, int enableInversion);

/**
 * @brief Create a Random Cube Vector 
 * 
 * @param basisPoint The center point to base the values around.
 * @param scaleFactor The range of the values from around the basis point.
 * @param enableInversion Enable inverting the polarity.
 * @return Vec3f The random cube vector.
 */
Vec3f createRandomCubeVector(float basisPoint[3], float scaleFactor, int enableInversion);
/**
 * @brief Create a Random Sphere Vector
 * 
 * @param basisPoint The center point to base the values around.
 * @param scaleFactor The range of the values from around the basis point.
 * @param enableInversion Enable inverting the polarity.
 * @return Vec3f The random cube vector.
 */
Vec3f createRandomSphereVector(float basisPoint[3], float scaleFactor, int enableInversion);

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
    enum ShapeType type;
    float clusterDistRatio;
    float percentBodiesInClusters;
    float scaleFactor;
    float maximumMass;
    float velocityMultipler;
    char* outputFile;
    size_t numBodies;
    size_t numClusters;
    size_t numFreeBodies;
    size_t numBodiesInClusters;
    size_t constantIndex;
    Body *bodies;
    Vec3f (*createFunction)(float[], float, int);
    
    //Check Arguments
    if(argc != 10) {
        printf("Invalid input format. Format via: globalSize[0,MAX_FLOAT], ");
        printf("numBodies[n], numClusters[n], percentBodiesInClusters[0,1], clusterDistRatio[0,1], ");
        printf("maximumMass[0,MAX_FLOAT], velocityMultiplier[0,1], outputFile, Type\n");
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
    outputFile = argv[8];
    type = atoi(argv[9]);
    scaleFactor = globalSize * clusterDistRatio;

    if(type == Sphere) {
        createFunction = &createRandomSphereVector;
    } else {
        createFunction = &createRandomCubeVector;
    }

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

        posVector = createFunction((float[]) {0, 0, 0}, scaleFactor, 1);
        velVector = createFunction((float[]) {0, 0, 0}, scaleFactor * velocityMultipler, 1);
        
        b.mass = ((float) rand() / (float) (RAND_MAX)) * maximumMass;
        b.pos = posVector;
        b.vel = velVector;

        bodies[constantIndex] = b;
    }
    
    //Calculate clusters
    for(int i = 0; i < numClusters; i++) {
        Vec3f basisVector;
        do {
            basisVector = createFunction((float[]){0, 0, 0}, globalSize, 1);
        } while(sqrtf(vectorDot(basisVector, basisVector)) > globalSize);

        for(int j = 0; j < (numBodiesInClusters / numClusters) + (numBodiesInClusters % numClusters < i ? 0 : 1); j++) {
            printf("Having these prints make it so it will export when < 4085\n");
            Vec3f posVector;
            Vec3f velVector;
            Body b;
            
            posVector = createFunction((float[]) {basisVector.x, basisVector.y, basisVector.z}, scaleFactor, 1);
            velVector = createFunction((float[]) {0, 0, 0}, scaleFactor * velocityMultipler, 1);

            b.mass = ((float) rand() / (float) (RAND_MAX)) * maximumMass;
            b.pos = posVector;
            b.vel = velVector;
            
            bodies[constantIndex] = b;
            constantIndex++;
        }
    }
    
    //Export as a text file.
    writeBodies(outputFile, bodies, numBodies);

    //Free memory
    free(bodies);
    return 0;
}

Vec3f createRandomSphereVector(float basisPoint[3], float scaleFactor, int enableInversion) {
    Vec3f retVector;
    do {
        retVector.x = createRandom(basisPoint[0], scaleFactor, enableInversion);
        retVector.y = createRandom(basisPoint[1], scaleFactor, enableInversion);
        retVector.z = createRandom(basisPoint[2], scaleFactor, enableInversion);
    } while(sqrtf(vectorDot(retVector, retVector)) > globalSize);
    return retVector;
}

Vec3f createRandomCubeVector(float basisPoint[3], float scaleFactor, int enableInversion) {
    Vec3f retVector;
    retVector.x = createRandom(basisPoint[0], scaleFactor, enableInversion);
    retVector.y = createRandom(basisPoint[1], scaleFactor, enableInversion);
    retVector.z = createRandom(basisPoint[2], scaleFactor, enableInversion);
    return retVector;
}

float createRandom(float basisPoint, float scaleFactor, int enableInversion) {
    float ret; 
    do {
        ret = (((float) rand() / (float) (RAND_MAX)) * scaleFactor);
        ret *= enableInversion ? ((rand() % 2) ? -1 : 1) : 1;
        ret += basisPoint;
    } while(abs(ret) > globalSize);
    return ret;
}