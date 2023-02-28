#!/bin/sh

# fail fast
set -e

SCRATCH_PATH=${SCRATCH_PATH:-${PWD}/deps}

# Clone the repo
mkdir -p deps
[ -d deps/folly ] || git clone https://github.com/facebook/folly deps/folly
cd deps/folly

# Build, using system dependencies if available, and some custom flags for arm64
python3 ./build/fbcode_builder/getdeps.py --allow-system-packages --scratch-path "${SCRATCH_PATH}" build --extra-cmake-defines='{"CMAKE_LIBRARY_ARCHITECTURE":"arm64", "CMAKE_CXX_FLAGS":"-fcoroutines-ts"}'
cd ..

echo """
########################################
Folly and dependencies installed in:
\"${SCRATCH_PATH}/installed\"

You can now run cmake with:
mkdir build
cd build/
cmake -DFOLLY_INSTALLED_AT:PATH=${SCRATCH_PATH}/installed ..
make
"""
