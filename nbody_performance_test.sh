#! /bin/bash

#SBATCH --job-name=nbody_performance_test
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=nbody_performance_test.out
#SBATCH --gres=gpu:1 # Require GPU capable nodes

#SBATCH --exclusive # Exclusive use of node

./cmake/bin/cmake .
make serial_direct_sum
#make serial_barnes_hut
#make parallel_barnes_hut
make nyland_cuda_implementation
make burtscher_cuda_implementation

TRIALS=$(seq 1 2)

for TRIAL in ${TRIALS}
do
    echo "Burtscher et. al. Trial $TRIAL"
    ./burtscher_cuda_implementation 65536 300 0 input.csv
done

for TRIAL in ${TRIALS}
do
    echo "Nyland et. al. Trial $TRIAL"
    ./nyland_cuda_implementation --f=input.csv
done

for TRIAL in ${TRIALS}
do
    echo "Serial Direct Sum Trial $TRIAL"
    ./serial_direct_sum input.csv 65536 300 1.0
done

# Uncomment when implemented
#for TRIAL in ${TRIALS}
#do
#    echo "Serial Barnes-Hut Trial $TRIAL"
#    ./serial_barnes_hut input.csv 65536 300 1.0
#done

# Uncomment when implemented
#for TRIAL in ${TRIALS}
#do
#    echo "Parallel Barnes-Hut Trial $TRIAL"
#    ./parallel_barnes_hut input.csv 65536 300 1.0
#done
