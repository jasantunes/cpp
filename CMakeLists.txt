cmake_minimum_required(VERSION 3.5)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)

# run cmake with something like:
# mkdir -r build; cd build; cmake -DFOLLY_INSTALLED_AT:PATH=${PWD}/../deps/installed ..
SET(FOLLY_INSTALLED_AT "folly_installed_at" CACHE PATH "Path to folly installation.")

project(cpp VERSION 0.0.2 LANGUAGES CXX)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # uses -std=c++20 instead of -std=gnu++20
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++ -lc++abi")

execute_process(
    COMMAND brew --prefix
    RESULT_VARIABLE BREW
    OUTPUT_VARIABLE BREW_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (BREW EQUAL 0 AND EXISTS "${BREW_PREFIX}")
    message(STATUS "Found homebrew at ${BREW_PREFIX}")
    include_directories("${BREW_PREFIX}/include")
    link_directories(${BREW_PREFIX}/lib)
endif()

find_package(PkgConfig REQUIRED)
# pkg_check_modules(glog REQUIRED libglog)
pkg_check_modules(event REQUIRED libevent)
find_package(Boost COMPONENTS system context thread REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
# find_package(gflags REQUIRED)
find_package(double-conversion REQUIRED)
find_package(GTest REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package (glog REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

# Folly and its deps
set(FOLLY_LIBS Threads::Threads double-conversion event gtest ${Boost_LIBRARIES})
set(FOLLY_DEPS "folly;fmt*;gflags*;glog*;googletest*")
foreach(DIRNAME ${FOLLY_DEPS})
    file(GLOB TMP_LIBS "${FOLLY_INSTALLED_AT}/${DIRNAME}/lib/lib*.dylib" "${FOLLY_INSTALLED_AT}/${DIRNAME}/lib/lib*.a")
    list(GET TMP_LIBS 0 FIRST_LIB)
    list(APPEND FOLLY_LIBS "${FIRST_LIB}")

    list(APPEND FOLLY_INCLUDES "${FOLLY_INSTALLED_AT}/${DIRNAME}/include")

    cmake_path(GET FIRST_LIB FILENAME LIB_FILE)
    message(STATUS "Found lib: ${LIB_FILE}")
endforeach()
message(VERBOSE "FOLLY_LIBS=${FOLLY_LIBS}")

# Folly's benchmark
find_file(FOLLY_BENCHMARK
  NAMES "Benchmark.cpp"
  PATHS "${FOLLY_INSTALLED_AT}/../folly/folly"
  NO_DEFAULT_PATH
  REQUIRED)
message("FOLLY_BENCHMARK=${FOLLY_BENCHMARK}")


add_executable(00_sandbox src/00_sandbox.cpp)

add_executable(01_style src/01_style.cpp)

add_executable(02_copy_and_move src/02_copy_and_move.cpp)

add_executable(03_smart_pointers src/03_smart_pointers.cpp)

add_executable(04_std_coroutines src/04_std_coroutines.cpp)
target_compile_options(04_std_coroutines PRIVATE -fcoroutines-ts)

add_executable(05_folly_coroutines_and_futures src/05_folly_coroutines_and_futures.cpp)
target_compile_options(05_folly_coroutines_and_futures PRIVATE -fcoroutines-ts)
target_include_directories(05_folly_coroutines_and_futures PRIVATE ${FOLLY_INCLUDES})
target_link_libraries(05_folly_coroutines_and_futures PRIVATE ${FOLLY_LIBS})

add_executable(06_folly_coroutines src/06_folly_coroutines.cpp)
target_compile_options(06_folly_coroutines PRIVATE -fcoroutines-ts)
target_include_directories(06_folly_coroutines PRIVATE ${FOLLY_INCLUDES})
target_link_libraries(06_folly_coroutines PRIVATE ${FOLLY_LIBS} log4cxx)

add_executable(07_templates src/07_templates.cpp)

add_executable(08_folly_optional src/08_folly_optional.cpp)
target_compile_options(08_folly_optional PRIVATE -fcoroutines-ts)
target_include_directories(08_folly_optional PRIVATE ${FOLLY_INCLUDES})
target_link_libraries(08_folly_optional PRIVATE ${FOLLY_LIBS})

add_executable(09_strings src/09_strings.cpp)
target_link_libraries(09_strings PRIVATE fmt::fmt magic_enum::magic_enum)
# https://clang.llvm.org/docs/AddressSanitizer.html#how-to-build
target_compile_options(09_strings PRIVATE -O1 -g -fsanitize=address -fno-omit-frame-pointer)
target_link_options(09_strings PRIVATE -g -fsanitize=address)

add_executable(10_benchmark src/10_benchmark.cpp ${FOLLY_BENCHMARK})
target_include_directories(10_benchmark PRIVATE ${FOLLY_INCLUDES})
target_link_libraries(10_benchmark PRIVATE ${FOLLY_LIBS})
target_compile_options(10_benchmark PRIVATE -O3 -DNDEBUG)

add_executable(11_logging_glog src/11_logging_glog.cpp)
target_link_libraries(11_logging_glog PRIVATE glog::glog)

add_executable(12_logging_folly src/12_logging_folly.cpp)
target_include_directories(12_logging_folly PRIVATE ${FOLLY_INCLUDES})
target_link_libraries(12_logging_folly PRIVATE fmt::fmt ${FOLLY_LIBS})

add_executable(13_google_test src/13_google_test.cpp)
target_link_libraries(13_google_test PRIVATE GTest::gtest GTest::gmock GTest::gmock_main)

add_executable(14_range src/14_range.cpp ${FOLLY_BENCHMARK})
target_include_directories(14_range PRIVATE ${FOLLY_INCLUDES})
target_link_libraries(14_range PRIVATE ${FOLLY_LIBS})

add_executable(15_functional src/15_functional.cpp)

