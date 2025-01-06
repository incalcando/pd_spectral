# set minimal version of cmake
cmake_minimum_required(VERSION 3.18)

project(sl_spectral)

add_compile_definitions(OBJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(Sample=float)

add_subdirectory(${PROJECT_NAME}~/${PROJECT_NAME}_dsp)


# # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
# set(FFTW_CFLAGS "-fPIC" CACHE STRING "FFTW CFLAGS")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FFTW_CFLAGS}")


include_directories(${PROJECT_SOURCE_DIR}/${PROJECT_NAME}~/${PROJECT_NAME}_dsp/include)
include_directories(${PROJECT_SOURCE_DIR}/${PROJECT_NAME}~/${PROJECT_NAME}_dsp/spectral-dsp/idsp/include)
include_directories(${PROJECT_SOURCE_DIR}/${PROJECT_NAME}~/${PROJECT_NAME}_dsp/spectral-dsp/include)

# import pd.cmake file
set(PDCMAKE_DIR ${PROJECT_SOURCE_DIR}/pd.cmake)
include(${PDCMAKE_DIR}/pd.cmake)

pd_add_external(${PROJECT_NAME}~ ${PROJECT_NAME}~/${PROJECT_NAME}~.cpp TARGET ${PROJECT_NAME}_tilde)
target_link_libraries(${PROJECT_NAME}_tilde PRIVATE ${PROJECT_NAME}_dsp)
pd_add_datafile(${PROJECT_NAME}_tilde ${PROJECT_NAME}~/${PROJECT_NAME}~-help.pd)

target_link_libraries(${PROJECT_NAME}_tilde PRIVATE spectral) # Link with fftw3f library
