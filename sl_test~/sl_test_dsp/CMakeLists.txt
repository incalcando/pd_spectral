
# set minimal version of cmake
cmake_minimum_required(VERSION 3.18)

# set the project name
project(sl_test_dsp)

# add source files
file(GLOB SOURCES src/*.cpp)

# add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/idsp/include)

# add target library
add_library(sl_test_dsp STATIC ${SOURCES})

