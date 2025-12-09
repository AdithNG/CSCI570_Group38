#!/bin/bash

# Stop if wrong number of arguments
if [ $# -ne 2 ]; then
    echo "Usage: ./basic.sh input.txt output.txt"
    exit 1
fi

# Compile the program
g++ basic.cpp -o basic -O2

# Run the program with given input & output paths
./basic "$1" "$2"
