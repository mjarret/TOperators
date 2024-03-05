#!/bin/sh

## Give your job a name to distinguish it from other jobs you run.
#SBATCH --job-name=T_03031559

## General partitions: all-HiPri, bigmem-HiPri   --   (12 hour limit)
##                     all-LoPri, bigmem-LoPri, gpuq  (5 days limit)
## Restricted: CDS_q, CS_q, STATS_q, HH_q, GA_q, ES_q, COS_q  (10 day limit)
##SBATCH --partition=bigmem
#SBATCH --partition=normal

## Separate output and error messages into 2 files.`
## NOTE: %u=userID, %x=jobName, %N=nodeID, %j=jobID, %A=arrayID, %a=arrayTaskID`
#SBATCH --output=/scratch/mjarretb/T_03031559/output/%j.out
#SBATCH --error=/scratch/mjarretb/T_03031559/output/%j.err

## Slurm can send you updates via email
#SBATCH --mail-type=FAIL         # ALL,NONE,BEGIN,END,FAIL,REQUEUE,..
#SBATCH --mail-user=mjarretb@gmu.edu     # Put your GMU email address here

## Specify how much memory your job needs. (2G is the default)
##SBATCH --mem=4084000MB        # Total memory needed per task (units: K,M,G,T)
#SBATCH --mem=128G

## Specify how much time your job needs. (default: see partition above)
#SBATCH --time=5-00:00  # Total time needed for job: Days-Hours:Minutes

## Specify number CPUs
##SBATCH --cpus-per-task 128
#SBATCH --nodes=1 --exclusive

## Load the relevant modules needed for the job
module load boost

##export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

## Run your program or script
/scratch/mjarretb/T_03031559/main.out -t 10 -d 7 --cases true 
