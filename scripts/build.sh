#!/bin/bash

# How to choose default compiler:
# sudo update-alternatives --config c++

mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=deploy ..
cmake --build . --config Debug
cmake -DBUILD_TYPE=Debug -P cmake_install.cmake
