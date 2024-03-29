#bin/bash

make clean

make makeT

# Check if the build was successful
if [ -f "main.out" ]; then
    echo "Build successful, running the program..."
    ./main.out
else
    echo "Build failed, exiting."
fi
