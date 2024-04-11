#!/bin/bash
brew update
brew upgrade
brew install llvm cmake git boost libomp
brew install numpy scipy python-matplotlib

git pull
cmake -S ./ -B ./build -DCMAKE_C_COMPILER=$(brew --prefix llvm)/bin/clang -DCMAKE_CXX_COMPILER=$(brew --prefix llvm)/bin/clang++
cmake --build ./build --config Release -j4
