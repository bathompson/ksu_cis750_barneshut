/*
 * Copyright 1993-2006 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:   
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and 
 * international Copyright laws.  
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE 
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR 
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.   
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, 
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS 
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOURCE CODE.  
 *
 * U.S. Government End Users.  This source code is a "commercial item" as 
 * that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting  of 
 * "commercial computer software" and "commercial computer software 
 * documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995) 
 * and is provided to the U.S. Government only as a commercial end item.  
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the 
 * source code with only those rights set forth herein.
 */

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <math.h>
#include <cutil.h>

#include "bodysystemcuda.h"
#include "bodysystemcpu.h"

// view params
int ox = 0, oy = 0;
int buttonState = 0;
float camera_trans[] = {0, -2, -100};
float camera_rot[]   = {0, 0, 0};
float camera_trans_lag[] = {0, -2, -100};
float camera_rot_lag[] = {0, 0, 0};
const float inertia = 0.1;

bool displayEnabled = true;
bool bPause = false;
enum { M_VIEW = 0, M_MOVE };

#ifdef __DEVICE_EMULATION__
int numBodies = 1024;
#else
int numBodies = 4096;
#endif

int numIterations = 0; // run until exit

float clusterScale = 1.54f;
float velocityScale = 8.f;
float damping = 1.0f; // no damping by default
float softening = 0.1f;
float timestep = 0.016f;
BodySystem *nbody = 0;
BodySystemCUDA *nbodyCUDA = 0;
BodySystemCPU  *nbodyCPU = 0;
float* hPos = 0;
float* hVel = 0;
float* hColor = 0;

// fps
static int fpsCount = 0;
static int fpsLimit = 1;
unsigned int timer = 0;

void reset(BodySystem *system, int numBodies, NBodyConfig config)
{
    // initalize the memory
    randomizeBodies(config, hPos, hVel, hColor, clusterScale, velocityScale, numBodies);

    system->setArray(BodySystem::BODYSYSTEM_POSITION, hPos);
    system->setArray(BodySystem::BODYSYSTEM_VELOCITY, hVel);
}

void init(int numBodies, int p, int q)
{
    nbodyCUDA = new BodySystemCUDA(numBodies, p, q);
    nbody = nbodyCUDA;

    // allocate host memory
    hPos = new float[numBodies*4];
    hVel = new float[numBodies*4];
    hColor = new float[numBodies*4];

    nbody->setSoftening(softening);
    nbody->setDamping(damping);

    CUT_SAFE_CALL(cutCreateTimer(&timer));
}

void compareResults(bool regression, int numBodies)
{
    nbodyCUDA->update(timestep); 

    // check result
    if(regression) 
    {
        // write file for regression test
        CUT_SAFE_CALL( cutWriteFilef( "./data/regression.dat",
            nbodyCUDA->getArray(BodySystem::BODYSYSTEM_POSITION), 
            numBodies, 0.0));
    }
    else
    {
        nbodyCPU = new BodySystemCPU(numBodies);

        nbodyCPU->setArray(BodySystem::BODYSYSTEM_POSITION, hPos);
        nbodyCPU->setArray(BodySystem::BODYSYSTEM_VELOCITY, hVel);

        nbodyCPU->update(timestep);

        float* cudaPos = nbodyCUDA->getArray(BodySystem::BODYSYSTEM_POSITION);
        float* cpuPos  = nbodyCPU->getArray(BodySystem::BODYSYSTEM_POSITION);

        // custom output handling when no regression test running
        // in this case check if the result is equivalent to the expected soluion
        CUTBoolean res = cutComparefe( cpuPos, cudaPos, numBodies, .01f);
        printf( "Test %s\n", (1 == res) ? "PASSED" : "FAILED");
    }
}

void computePerfStats(double &interactionsPerSecond, double &gflops, float milliseconds, int iterations)
{
    const int flopsPerInteraction = 20;
    interactionsPerSecond = (float)numBodies * (float)numBodies;
    interactionsPerSecond *= 1e-9 * iterations * 1000 / milliseconds;
    gflops = interactionsPerSecond * (float)flopsPerInteraction;

}

void runBenchmark(int iterations)
{
    // once without timing to prime the GPU
    nbody->update(timestep);

    CUT_SAFE_CALL(cutStartTimer(timer));  
    for (int i = 0; i < iterations; ++i)
    {
        nbody->update(timestep);
    }
    CUT_SAFE_CALL(cutStopTimer(timer));  

    float milliseconds = cutGetTimerValue(timer);
    double interactionsPerSecond = 0;
    double gflops = 0;
    computePerfStats(interactionsPerSecond, gflops, milliseconds, iterations);
    
    printf("%d bodies, total time for %d iterations: %0.3f ms\n", 
           numBodies, iterations, milliseconds);
    printf("= %0.3f billion interactions per second\n", interactionsPerSecond);
    printf("= %0.3f GFLOP/s at %d flops per interaction\n", gflops, 20);
    
}

////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////
int
main( int argc, char** argv) 
{
    numIterations = 0;
    int p = 256;
    int q = 1;

    cutGetCmdLineArgumenti( argc, (const char**) argv, "n", &numBodies);

    switch (numBodies)
    {
    case 1024:
        clusterScale = 1.52f;
        velocityScale = 2.f;
        break;
    case 2048:
        clusterScale = 1.56f;
        velocityScale = 2.64f;
        break;
    case 4096:
        clusterScale = 1.68f;
        velocityScale = 2.98f;
        break;
    case 8192:
        clusterScale = 1.98f;
        velocityScale = 2.9f;
        break;
    case 16384:
        clusterScale = 1.54f;
        velocityScale = 8.f;
        break;
    case 32768:
        clusterScale = 1.44f;
        velocityScale = 11.f;
        break;
    }



    bool benchmark = cutCheckCmdLineFlag(argc, (const char**) argv, "benchmark") != 0;
    cutGetCmdLineArgumenti( argc, (const char**) argv, "i", &numIterations);
    cutGetCmdLineArgumenti( argc, (const char**) argv, "p", &p);
    cutGetCmdLineArgumenti( argc, (const char**) argv, "q", &q);

    bool compareToCPU = cutCheckCmdLineFlag( argc, (const char**) argv, "compare") != 0;
    bool regression = cutCheckCmdLineFlag( argc, (const char**) argv, "regression") != 0;

    //initGL();
    init(numBodies, p, q);
    //initParameters();
    
    reset(nbody, numBodies, NBODY_CONFIG_SHELL);

    if (compareToCPU || regression)
    {
        compareResults(regression, numBodies);
        return 0;
    }

    if (/*benchmark*/ true)
    {
        if (numIterations <= 0) 
            numIterations = 300;
        runBenchmark(numIterations);
    }

    if (nbodyCPU)
        delete nbodyCPU;
    if (nbodyCUDA)
        delete nbodyCUDA;

    if (hPos)
        delete [] hPos;
    if (hVel)
        delete [] hVel;
    if (hColor)
        delete [] hColor;

    return 0;
}
