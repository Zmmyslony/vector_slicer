#!/bin/bash

brew install llvm cmake git boost libomp

# Export LLVM flags
export LDFLAGS=$(brew --prefix llvm)/lib
export CPPFLAGS=$(brew --prefix llvm)/include

#Export libomp
export LDFLAGS=$(brew --prefix libomp)/lib
export CPPFLAGS=$(brew --prefix libomp)/include

git pull
cmake -S ./ -B ./build -DCMAKE_C_COMPILER=$(brew --prefix llvm)/bin/clang -DCMAKE_CXX_COMPILER=$(brew --prefix llvm)/bin/clang++
cmake --build ./build --config Release -j4

echo
echo
echo "For automatic detection of the api location please add to your ~/.bashrc following lines:"
echo "    export VECTOR_SLICER_OUTPUT=\"$PWD/output\""
echo "    export VECTOR_SLICER_API=\"$PWD/build/libvector_slicer_api.dylib\""
echo
