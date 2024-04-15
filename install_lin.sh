#!/bin/bash

sudo apt update
sudo apt upgrade
sudo apt install cmake libboost-all-dev

git pull
cmake -S ./ -B ./build
cmake --build ./build --config Release -j4


