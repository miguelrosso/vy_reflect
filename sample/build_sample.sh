#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

g++ ../src/*.cpp sample.cpp --std=c++17 --debug -Wall -o build/vy_reflection_sample
