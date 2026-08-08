#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Cleaning..."

mkdir -p "$PROJECT_ROOT/build"
rm -rf "$PROJECT_ROOT/build"/*

echo "Building project..."

cmake -S "$PROJECT_ROOT" -B "$PROJECT_ROOT/build" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$PROJECT_ROOT/build" -j

echo "Done."
