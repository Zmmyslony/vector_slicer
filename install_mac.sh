#!/bin/bash

brew install cmake git boost libomp

git pull
cmake -S ./ -B ./build
cmake --build ./build --config Release -j4

echo
echo
echo "For automatic detection of the api location please add to your ~/.bashrc following lines:"
echo "    export VECTOR_SLICER_OUTPUT=\"$PWD/output\""
echo "    export VECTOR_SLICER_API=\"$PWD/build/libvector_slicer_api.dylib\""
echo
