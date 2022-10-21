cmake_minimum_required(VERSION 3.5)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)

# run cmake with something like:
# cmake -DFOLLY_INSTALLED_AT:PATH=/Users/jantunes/.scratch/UsersZjantunesZdevZfolly/fbcode_builder_getdeps/installed ..
SET(FOLLY_INSTALLED_AT "folly_installed_at" CACHE PATH "Path to folly installation.")

project(cpp VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # uses -std=c++20 instead of -std=gnu++20
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++ -lc++abi")

macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

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
# find_package(fmt CONFIG REQUIRED)
# find_package(gflags REQUIRED)
find_package(double-conversion REQUIRED)

# Folly and its deps
set(FOLLY_INCLUDES "")
subdirlist(FOLY_DEPS "${FOLLY_INSTALLED_AT}")
foreach(DIRNAME ${FOLY_DEPS})
    message(STATUS "at ${DIRNAME}/")

    list(APPEND FOLLY_INCLUDES "${FOLLY_INSTALLED_AT}/${DIRNAME}/include")

    file(GLOB TMP_LIBS "${FOLLY_INSTALLED_AT}/${DIRNAME}/lib/lib*.a" "${FOLLY_INSTALLED_AT}/${DIRNAME}/lib/lib*.dylib")
    list(GET TMP_LIBS 0 FIRST_LIB)
    message(STATUS "Found lib: ${FIRST_LIB}")
    list(APPEND FOLLY_LIBS "${FIRST_LIB}")
endforeach()

# add_executable(1_style      src/1_style.cpp)

add_executable(4_coroutines src/4_coroutines.cpp)
target_compile_options(4_coroutines PRIVATE "-fcoroutines-ts")


add_executable(4_coroutines_folly src/4_coroutines_folly.cpp)
target_compile_options(4_coroutines_folly PRIVATE "-fcoroutines-ts")
target_include_directories(4_coroutines_folly PRIVATE "${FOLLY_INCLUDES}")
target_link_libraries(
    4_coroutines_folly
    PRIVATE
    Threads::Threads
    double-conversion
    event
    ${FOLLY_LIBS}
    ${Boost_LIBRARIES}
)
