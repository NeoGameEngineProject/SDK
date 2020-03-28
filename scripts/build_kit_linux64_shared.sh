#!/bin/sh -e

mkdir -p build-linux64-shared-$1
cd build-linux64-shared-$1

export CC=clang
export CXX=clang++

cmake .. -G Ninja -DCMAKE_BUILD_TYPE="$1" -DNEO_BUILD_SHARED_LIBS=ON -DNO_DOCS=TRUE -DNO_TESTS=ON -DENABLE_OPENGL_RENDERER=ON -DNO_TESTGAME=ON -DENABLE_SDL2_BUILD=TRUE
ninja package
