#!/bin/sh

scp -r *.cpp mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp -r *.hpp mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp Makefile.cluster mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators/Makefile
scp -r runscript.slurm mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
scp -r patterns.txt mjarretb@hopper.orc.gmu.edu:~/scratch/TOperators
ssh mjarretb@hopper '/usr/bin/make -C ~/scratch/TOperators'
##ssh hopper 'sbatch ~/scratch/TOperators/runscript.slurm'
