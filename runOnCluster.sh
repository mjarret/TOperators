#!/bin/sh

scp -r *.cpp mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp -r *.hpp mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp -r Makefile mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp -r runscript.slurm mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp -r patterns.txt mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
ssh mjarretb@hopper '/usr/bin/make -C ~/TOperators'
ssh hopper 'sbatch ~/TOperators/runscript.slurm'
