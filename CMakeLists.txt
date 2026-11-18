cmake_minimum_required(VERSION 3.25)

project(falcon VERSION 0.1.0 LANGUAGES CXX C)

# Compilers
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_C_COMPILER "/usr/bin/gcc" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "/usr/bin/g++" CACHE FILEPATH "C++ compiler" FORCE)
endif()
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "C++ standard required: ${CMAKE_CXX_STANDARD_REQUIRED}")
message(STATUS "C++ extensions: ${CMAKE_CXX_EXTENSIONS}")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

# multi-configuration generators eg. Visual Studio, Xcode, Ninja Multi-Config
# cmake -S . -B build "-DCMAKE_CONFIGURATION_TYPES=Debug;Release"
# message(STATUS "Configuration types: ${CMAKE_CONFIGURATION_TYPES}")

# Default build type for single-configuration generators
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
    endif()

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug
        Release
        RelWithDebInfo
        MinSizeRel
    )

    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

# CPU information and suggested parallel job count
cmake_host_system_information(
    RESULT CPU_NUMS
    QUERY NUMBER_OF_LOGICAL_CORES
)
message(STATUS "Logical CPU cores: ${CPU_NUMS}")
math(EXPR JOBS "${CPU_NUMS} + ${CPU_NUMS} / 2")
# This cache entry does not control the parallelism of cmake --build
set(CMAKE_BUILD_PARALLEL_LEVEL ${JOBS} CACHE STRING "Parallel build job count")
message(STATUS "Cached parallel jobs: ${CMAKE_BUILD_PARALLEL_LEVEL}")

# Common build settings
include(cmake/falconOptions.cmake)

# Main application
add_subdirectory(falcon)

# Build options
option(FALCON_BUILD_EXAMPLES "Build examples" ON)
option(FALCON_BUILD_TESTS "Build tests" ON)
option(FALCON_BUILD_TOOLS "Build tools" ON)

# Tests
if(FALCON_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
# if(FALCON_BUILD_EXAMPLES)
#     add_subdirectory(examples)
# endif()

# Tools
if(FALCON_BUILD_TOOLS)
    add_subdirectory(tools)
endif()