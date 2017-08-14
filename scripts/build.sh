#!/bin/bash

mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=deploy ..
cmake --build . --config Debug
cmake -DBUILD_TYPE=Debug -P cmake_install.cmake
