#!/bin/bash

mkdir -p build
mkdir -p logs

cd build

echo "🔧 Running CMake..."
cmake .. > ../logs/build_errors.log 2>&1

echo "🔨 Building project..."
make >> ../logs/build_errors.log 2>&1

cd ..

if [[ -f build/test_suite ]]; then
    echo "🧪 Running test suite..."
    ./build/test_suite > logs/test_output.log 2>&1
else
    echo "❌ Build failed. See logs/build_errors.log"
fi
