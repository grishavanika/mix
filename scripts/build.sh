#!/bin/bash

# How to switch to clang:
#
# export CC=/usr/bin/clang-5.0
# export CXX=/usr/bin/clang++-5.0

mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=deploy ..
cmake --build . --config Debug
cmake -DBUILD_TYPE=Debug -P cmake_install.cmake
