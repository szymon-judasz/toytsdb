#!/bin/bash

# Build script for the project
# This script will compile the project and place output in the build directory

echo "Cleaning..."

mkdir -p build
rm -rf build/*

echo "Building project..."

cmake -S .. -B build
cmake --build build -j

echo "Done."
