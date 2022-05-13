#! /bin/bash

#SBATCH --job-name=theta_value_test
#SBATCH --time=02:00:00
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --output=theta_value_test.out
#SBATCH --gres=gpu:1 # Require GPU capable nodes

#SBATCH --exclusive # Exclusive use of node

./cmake/bin/cmake .
make serial_direct_sum
make serial_barnes_hut

echo "Serial Direct Sum"
./serial_direct_sum input.csv 8096 150 1.0 outputdirect.nbody 1000000

echo "Theta = 0.05"
./serial_barnes_hut input.csv 8096 150 1.0 0.05 1000000 output005.nbody

echo "Theta = 0.1"
./serial_barnes_hut input.csv 8096 150 1.0 0.1 1000000 output010.nbody

echo "Theta = 0.2"
./serial_barnes_hut input.csv 8096 150 1.0 0.2 1000000 output020.nbody

echo "Theta = 0.3"
./serial_barnes_hut input.csv 8096 150 1.0 0.3 1000000 output030.nbody

echo "Theta = 0.4"
./serial_barnes_hut input.csv 8096 150 1.0 0.4 1000000 output040.nbody

echo "Theta = 0.5"
./serial_barnes_hut input.csv 8096 150 1.0 0.5 1000000 output050.nbody
