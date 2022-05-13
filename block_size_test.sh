#! /bin/bash

#SBATCH --job-name=block_size_test
#SBATCH --time=02:00:00
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=block_size_test.out
#SBATCH --gres=gpu:1 # Require GPU capable nodes

#SBATCH --exclusive # Exclusive use of node

./cmake/bin/cmake -DCMAKE_BUILD_TYPE=Release .
make parallel_barnes_hut

TRIALS=$(seq 1 5)

# This test requires changing the source code for each run
for TRIAL in ${TRIALS}
do
  echo "Block Size Test"
  ./serial_barnes_hut input.csv 8192 150 1.0 0.05 1000000 output005.nbody
done