#!/bin/bash


# Generate timestamp first to use in defaults
TIMESTAMP=$(date "+%m%d%H%M")
TARGET_DIR="/scratch/mjarretb/T_${TIMESTAMP}"

# Set initial defaults that depend on TIMESTAMP
job_name="T_${TIMESTAMP}"
partition="normal"
output="${TARGET_DIR}/output/%j.out"
error_file="${TARGET_DIR}/output/%j.err"

# Parse command-line options
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --job-name) job_name="$2"; shift ;;
        --partition) partition="$2"; shift ;;
        --output) output="$2"; shift ;;
        --error_file) error_file="$2"; shift ;;
        --) shift; break ;; # End of options signal
        *)  # Collect runtime options
            if [[ $1 != --* ]]; then
                runtime_options+="$1 "
            else
                echo "Unknown option: $1"
                exit 1
            fi
            ;;
    esac
    shift
done

# Check if runtime options were provided
if [ -z "$runtime_options" ]; then
    echo "Runtime options are required."
    exit 1
fi

# Echo the parameters for review
echo "The following parameters will be used to modify the Slurm script:"
echo "Job Name: $job_name"
echo "Partition: $partition"
echo "Output File: $output"
echo "Error File: $error_file"
echo "Runtime Options: $runtime_options"

# Ask the user for confirmation
read -p "Do you want to proceed with these settings? (y/n) " confirmation
if [[ $confirmation != "y" && $confirmation != "Y" ]]; then
    echo "Operation cancelled by the user."
    exit 1
fi


# Modify the Slurm script
sed -i "s/^#SBATCH --job-name=.*/#SBATCH --job-name=${job_name}/" runscript.slurm
sed -i "s/^#SBATCH --partition=.*/#SBATCH --partition=${partition}/" runscript.slurm
sed -i "s|^#SBATCH --output=.*|#SBATCH --output=${output}|" runscript.slurm
sed -i "s|^#SBATCH --error=.*|#SBATCH --error=${error_file}|" runscript.slurm
sed -i "/main\.out/c${TARGET_DIR}/main.out ${runtime_options}" runscript.slurm

USER=mjarretb
HOST=hopper.orc.gmu.edu

# Check if the target directory already exists on the remote server
if ssh ${USER}@${HOST} "[ -d ${TARGET_DIR} ]"; then
    echo "Target directory ${TARGET_DIR} already exists. Exiting script."
    exit 1
else
    # Create the target directory structure
    ssh ${USER}@${HOST} "mkdir -p ${TARGET_DIR}/data/reductions ${TARGET_DIR}/output ${TARGET_DIR}/cases ${TARGET_DIR}/patterns"
fi
# Initialize rsync_status with a value that will enter the loop
rsync_status=30
while [ $rsync_status -eq 30 ]; do
    # Run rsync with a timeout
    rsync -v --timeout=15 --exclude-from='exclude.txt' ./* ${USER}@${HOST}:${TARGET_DIR}/

    # Capture the exit status of rsync
    rsync_status=$?

    if [ $rsync_status -eq 30 ]; then
        echo "rsync operation timed out. Sleeping for 3 seconds before retrying..."
        sleep 3
    elif [ $rsync_status -ne 0 ]; then
        echo "rsync failed with exit status $rsync_status."
        # Exit the script or handle the error as needed
        exit $rsync_status
    fi
done

# Rename Makefile.cluster to Makefile on the remote server
# Run make and sbatch commands on the remote server
ssh -t ${USER}@${HOST} << EOF
mv ${TARGET_DIR}/Makefile.cluster ${TARGET_DIR}/Makefile
module load boost
/usr/bin/make -C ${TARGET_DIR}
EOF
sleep 3

ssh -t ${USER}@${HOST} "sbatch ${TARGET_DIR}/runscript.slurm"
