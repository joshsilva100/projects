#!/bin/bash

#SBATCH --job-name silva_vector
#SBATCH --nodes 1
#SBATCH --tasks-per-node 1
#SBATCH --cpus-per-task 1
#SBATCH --mem 16gb
#SBATCH --time 01:00:00
#SBATCH --constraint=chip_type_9654 

module load gcc 
module load cmake