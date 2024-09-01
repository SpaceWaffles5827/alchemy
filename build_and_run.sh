#!/bin/bash

# Generate Makefiles with Premake
premake5 gmake2

# Compile the project
make -j$(sysctl -n hw.ncpu)

# Run the application
./bin/Debug/AlchemyProject  # Or ./bin/Debug/AlchemyProject

