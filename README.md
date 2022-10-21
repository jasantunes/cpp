# C++ Hello World
Hello world in C++.


# Dependencies

## Folly and deps

Unfortunately, folly's build scripts are not properly configured to support clang and arm64 (eg, macos M1), at least I couldn't get it to build unless I changed the cmake files directly.

If you have a macos arm64, run the following commands to build folly with coroutine support with xcode's clang.

```
git submodule update --init --recursive
cd third_party/folly
git am ../../patches/third_party/folly/*.patch
python3 ./build/fbcode_builder/getdeps.py \
  --allow-system-packages \
  --scratch-path build-aux \
  build

echo "Folly and deps installed at ${PWD}/build-aux/installed"
```
We're going to use this directory to run cmake later


### Save changes you made to Folly as local patches

```
cd third_party/folly
git format-patch -o ../../patches/folly main
```
> Note: to apply these patches, look at how to build "Folly and deps".

# Build

Make sure you have folly and its dependencies installed already. If you built folly locally (see Folly and deps), you'll need the path to the build.

```
cd build/
cmake -DFOLLY_INSTALLED_AT:PATH=/path/to/folly/build-aux/installed ..
make
```

