#! /bin/bash

#SBATCH --job-name=nbody_performance_test
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=square.out
#SBATCH --gres=gpu:1 # Require GPU capable nodes
#SBATCH --partition=ksu-gen-gpu.q # Priority access to GPU nodes
#SBATCH --exclusive # Exclusive use of node

./cmake/bin/cmake .
make serial_direct_sum
make serial_barnes_hut
make parallel_barnes_hut
make nyland_cuda_implementation
make burtscher_cuda_implementation

trials = 2

for (( i=1; i <= $trials; ++i ))
do
    echo "Serial Direct Sum Trial $i"
    ./serial_direct_sum input.csv 65536 300 1.0
done

for (( i=1; i <= $trials; ++i ))
do
    echo "Serial Barnes-Hut Trial $i"
    ./serial_barnes_hut input.csv 65536 300 1.0
done

# Uncomment when implemented
#for (( i=1; i <= $trials; ++i ))
#do
#    echo "Parallel Barnes-Hut Trial $i"
#    ./parallel_barnes_hut input.csv 65536 300 1.0
#done

# Uncomment when implemented
#for (( i=1; i <= $trials; ++i ))
#do
#    echo "Parallel Barnes-Hut Trial $i"
#    ./parallel_barnes_hut input.csv 65536 300 1.0
#done

for (( i=1; i <= $trials; ++i ))
do
    echo "Burtscher et. al. Trial $i"
    ./burtscher_cuda_implementation 65536 300 0 input.csv
done

for (( i=1; i <= $trials; ++i ))
do
    echo "Nyland et. al. Trial $i"
    ./nyland_cuda_implementation -f input.csv
done
