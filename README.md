# Build

## Folly and deps on arm64
Unfortunaely, Homebrew's folly doesn't build with coroutines support for macos arm64 (eg, macbook m1), which results in the usual __Undefined symbols for architecture arm64__ errors. So, if you have one of these machines, you'll have to build folly with the proper compiler flags.

You can pass cmake flags to the build script to build folly on macbook m1 (arm64) using:
```
# Clone the repo
mkdir deps
git clone https://github.com/facebook/folly deps/folly
cd deps/folly
# Build, using system dependencies if available, and some custom flags for arm64
python3 ./build/fbcode_builder/getdeps.py --allow-system-packages --scratch-path ../ build --extra-cmake-defines='{"DCMAKE_LIBRARY_ARCHITECTURE":"arm64", "CMAKE_CXX_FLAGS":"-fcoroutines-ts"}'
cd ..
```

This can take a while, but once the build script finishes, it should have built folly and its dependencies with the proper clang flags (eg, -stdlib=libc++ -fcoroutines-ts ...) in `./deps/installed`. You're going to use these libraries to link your binary (eg, `./deps/installed/fmt-9p6KSAawzA3WBge3_80F7LQZKwZBhTOECl-6eNjC8WE/lib/libfmt.a`).

This is basically what `build_folly_arm64.sh` does.

## cmake

To generate the binaries, you can use cmake:
```
cd build/
cmake -DFOLLY_INSTALLED_AT:PATH=${PWD}/../deps/installed ..
make
```
