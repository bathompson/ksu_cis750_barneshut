#! /bin/bash

#SBATCH --job-name=nbody_performance_test
#SBATCH --time=02:00:00
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=nbody_performance_test.out
#SBATCH --gres=gpu:1 # Require GPU capable nodes

#SBATCH --exclusive # Exclusive use of node

./cmake/bin/cmake .
make serial_direct_sum
make serial_barnes_hut
#make parallel_barnes_hut
make nyland_cuda_implementation
make burtscher_cuda_implementation

TRIALS=$(seq 1 1)

for TRIAL in ${TRIALS}
do
    echo "Burtscher et. al. Trial $TRIAL"
    ./burtscher_cuda_implementation 16384 300 0 input.csv
done

for TRIAL in ${TRIALS}
do
    echo "Nyland et. al. Trial $TRIAL"
    ./nyland_cuda_implementation --f=input.csv
done

for TRIAL in ${TRIALS}
do
    echo "Serial Direct Sum Trial $TRIAL"
    ./serial_direct_sum input.csv 16384 300 1.0 1000000 outputdirect.nbody
done

# Uncomment when implemented
for TRIAL in ${TRIALS}
do
    echo "Serial Barnes-Hut Trial $TRIAL"
    ./serial_barnes_hut input.csv 16384 300 1.0 0.5 1000000 outputbarnes.nbody
done

# Uncomment when implemented
#for TRIAL in ${TRIALS}
#do
#    echo "Parallel Barnes-Hut Trial $TRIAL"
#    ./parallel_barnes_hut input.csv 16384 300 1.0
#done
