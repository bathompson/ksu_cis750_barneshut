#! /bin/bash

#SBATCH --job-name=scalability_test
#SBATCH --time=01:00:00
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=scalability_test.out
#SBATCH --gres=gpu:1 # Require GPU capable nodes

#SBATCH --exclusive # Exclusive use of node

./cmake/bin/cmake -DCMAKE_BUILD_TYPE=Release .
make serial_barnes_hut
make parallel_barnes_hut
make nyland_cuda_implementation
make burtscher_cuda_implementation

echo "1024 Bodies"
echo "Serial Barnes-Hut"
./serial_barnes_hut input.csv 1024 150 1.0 0.05 1000000 output.nbody
echo "Our Parallel Barnes-Hut"
./serial_barnes_hut input.csv 1024 150 1.0 0.05 1000000 output.nbody
echo "Nyland Implementation"
-n=1024 -i=150 -f=input.csv
echo "Burtscher Implementation"
1024 150 0 input.csv

echo "2048 Bodies"
echo "Serial Barnes-Hut"
./serial_barnes_hut input.csv 2048 150 1.0 0.05 1000000 output.nbody
echo "Our Parallel Barnes-Hut"
./serial_barnes_hut input.csv 2048 150 1.0 0.05 1000000 output.nbody
echo "Nyland Implementation"
-n=2048 -i=150 -f=input.csv
echo "Burtscher Implementation"
2048 150 0 input.csv

echo "4096 Bodies"
echo "Serial Barnes-Hut"
./serial_barnes_hut input.csv 4096 150 1.0 0.05 1000000 output.nbody
echo "Our Parallel Barnes-Hut"
./serial_barnes_hut input.csv 4096 150 1.0 0.05 1000000 output.nbody
echo "Nyland Implementation"
-n=4096 -i=150 -f=input.csv
echo "Burtscher Implementation"
4096 150 0 input.csv

echo "8192 Bodies"
echo "Serial Barnes-Hut"
./serial_barnes_hut input.csv 8192 150 1.0 0.05 1000000 output.nbody
echo "Our Parallel Barnes-Hut"
./serial_barnes_hut input.csv 8192 150 1.0 0.05 1000000 output.nbody
echo "Nyland Implementation"
-n=8192 -i=150 -f=input.csv
echo "Burtscher Implementation"
8192 150 0 input.csv

echo "16384 Bodies"
echo "Serial Barnes-Hut"
./serial_barnes_hut input.csv 16384 150 1.0 0.05 1000000 output.nbody
echo "Our Parallel Barnes-Hut"
./serial_barnes_hut input.csv 16384 150 1.0 0.05 1000000 output.nbody
echo "Nyland Implementation"
-n=16384 -i=150 -f=input.csv
echo "Burtscher Implementation"
16384 150 0 input.csv

echo "32768 Bodies"
echo "Serial Barnes-Hut"
./serial_barnes_hut input.csv 32768 150 1.0 0.05 1000000 output.nbody
echo "Our Parallel Barnes-Hut"
./serial_barnes_hut input.csv 32768 150 1.0 0.05 1000000 output.nbody
echo "Nyland Implementation"
-n=32768 -i=150 -f=input.csv
echo "Burtscher Implementation"
32768 150 0 input.csv
