#!/bin/bash

# initialise a new git repository
git init || echo "Git repository already exists"

# Get the object name and type from the command line arguments
OBJECT_NAME="$1"
TYPE="$2"
SPECTRAL=0

# Check if the type is not specified, set SPECTRAL to 0
if [ -z "$TYPE" ]; then
    SPECTRAL=0
fi

# If the type is "spectral", set SPECTRAL to 1
if [ "$TYPE" = "spectral" ]; then
    SPECTRAL=1
fi

# Check if the object name is not specified, print an error message and exit
if [ -z "$OBJECT_NAME" ]; then
    echo "no object name specified, please specify an object name. Tilde character is not required."
    exit
fi

# Replace "Blank_Pd_Object" with the object name in CMakeLists.txt
sed -i "s/Blank_Pd_Object/$OBJECT_NAME/" CMakeLists.txt

# If SPECTRAL is 1, modify the include directories in CMakeLists.txt
if [ "$SPECTRAL" -eq 1 ]; then
    echo "spectral mode selected"
    sed -i "s|include_directories(\${PROJECT_SOURCE_DIR}/\${PROJECT_NAME}~/\${PROJECT_NAME}_dsp/idsp/include)|include_directories(\${PROJECT_SOURCE_DIR}/\${PROJECT_NAME}~/\${PROJECT_NAME}_dsp/spectral-dsp/idsp/include)\ninclude_directories(\${PROJECT_SOURCE_DIR}/\${PROJECT_NAME}~/\${PROJECT_NAME}_dsp/spectral-dsp/include)|" CMakeLists.txt
fi

# Replace "BlankPd" with the object name in the source and help files
sed -i "s/BlankPd/${OBJECT_NAME}/g" Blank_Pd_Object~/Blank_Pd_Object~.cpp
sed -i "s/\b${OBJECT_NAME}\b/\u&/g" Blank_Pd_Object~/Blank_Pd_Object~.cpp
sed -i "s/blank_pd/${OBJECT_NAME}/g" Blank_Pd_Object~/Blank_Pd_Object~.cpp
sed -i "s/blank_pd/${OBJECT_NAME}/" Blank_Pd_Object~/Blank_Pd_Object~-help.pd
sed -i "s/blank_pd/${OBJECT_NAME}/" .gitignore

# Replace "BlankPd" with the object name in the launch.jso and tasks.json files
sed -i "s/Blank_Pd_Object/${OBJECT_NAME}/g" .vscode/launch.json
sed -i "s/Blank_Pd_Object/${OBJECT_NAME}/g" .vscode/tasks.json

# Add the pd.cmake submodule
git submodule add https://github.com/InstruoModular/pd.cmake.git

# Rename the directories and files to use the object name
mv Blank_Pd_Object~ $OBJECT_NAME~
mv $OBJECT_NAME~/Blank_Pd_Object~.cpp $OBJECT_NAME~/$OBJECT_NAME~.cpp
mv $OBJECT_NAME~/Blank_Pd_Object~-help.pd $OBJECT_NAME~/$OBJECT_NAME~-help.pd
mkdir $OBJECT_NAME~/${OBJECT_NAME}_dsp
mkdir $OBJECT_NAME~/${OBJECT_NAME}_dsp/include
mkdir $OBJECT_NAME~/${OBJECT_NAME}_dsp/src

# If SPECTRAL is 0, add the idsp submodule, otherwise clone the spectral-dsp repository
if [ "$SPECTRAL" -eq 0 ]; then
    git submodule add -f https://github.com/InstruoModular/idsp.git  $OBJECT_NAME~/${OBJECT_NAME}_dsp/idsp
else
    git clone https://github.com/InstruoModular/spectral-dsp.git $OBJECT_NAME~/${OBJECT_NAME}_dsp/spectral-dsp
    cd $OBJECT_NAME~/${OBJECT_NAME}_dsp/spectral-dsp
    git submodule update --init --recursive
    cd ../../..
    touch $OBJECT_NAME~/${OBJECT_NAME}_dsp/.gitignore
    echo "spectral-dsp/" >> $OBJECT_NAME~/${OBJECT_NAME}_dsp/.gitignore
fi

# Generate the C++ source file for the DSP processing
cat <<EOL > $OBJECT_NAME~/${OBJECT_NAME}_dsp/src/${OBJECT_NAME}_dsp.cpp
#include "${OBJECT_NAME}_dsp.hpp"
#include "idsp/functions.hpp"

void ${OBJECT_NAME}::process(const DspBuffer& input, DspBuffer& output)
{
    // DSP processing code here
    for(size_t i = 0; i < input[0].size(); i++)
    {
        output[0][i] = input[0][i] * gain;
        output[1][i] = input[1][i] * gain;
    }
}
EOL

# Capitalize the class name in the source file
sed -i "s/\b${OBJECT_NAME}\b/\u&/g" $OBJECT_NAME~/${OBJECT_NAME}_dsp/src/${OBJECT_NAME}_dsp.cpp

# Generate the C++ header file for the DSP processing
cat <<EOL > $OBJECT_NAME~/${OBJECT_NAME}_dsp/include/${OBJECT_NAME}_dsp.hpp
#ifndef ${OBJECT_NAME}_DSP_HPP
#define ${OBJECT_NAME}_DSP_HPP

#include "idsp/buffer_types.hpp"

using DspBuffer = idsp::PolySampleBufferDynamic<2>;

class ${OBJECT_NAME}
{
    public:
        ${OBJECT_NAME}() : gain(1.0f)
        {}

        void process(const DspBuffer& input, DspBuffer& output);
        void set_gain(const float f) {gain = f;}

    private:
        float gain;
};

#endif // ${OBJECT_NAME}_DSP_HPP
EOL

# Capitalize the class name in the header file
sed -i "s/\b${OBJECT_NAME}\b/\u&/g" $OBJECT_NAME~/${OBJECT_NAME}_dsp/include/${OBJECT_NAME}_dsp.hpp

# Generate the nested CMakeLists.txt to compile the library
if [ "$SPECTRAL" -eq 0 ]; then
    cat <<EOL > $OBJECT_NAME~/${OBJECT_NAME}_dsp/CMakeLists.txt

# set minimal version of cmake
cmake_minimum_required(VERSION 3.18)

# set the project name
project(${OBJECT_NAME}_dsp)

# add source files
file(GLOB SOURCES src/*.cpp)

# add include directories
include_directories(\${PROJECT_SOURCE_DIR}/include)
include_directories(\${PROJECT_SOURCE_DIR}/idsp/include)

# add target library
add_library(${OBJECT_NAME}_dsp STATIC \${SOURCES})

EOL
else
    cat <<EOL > $OBJECT_NAME~/${OBJECT_NAME}_dsp/CMakeLists.txt

# set minimal version of cmake
cmake_minimum_required(VERSION 3.18)

# set the project name
project(${OBJECT_NAME}_dsp)

# add source files
file(GLOB SOURCES src/*.cpp)

# add include directories
include_directories(\${PROJECT_SOURCE_DIR}/include)
include_directories(\${PROJECT_SOURCE_DIR}/spectral-dsp/idsp/include)
include_directories(\${PROJECT_SOURCE_DIR}/spectral-dsp/include)

# add spectral-dsp subproject
add_subdirectory(spectral-dsp)

# add target library
add_library(${OBJECT_NAME}_dsp STATIC \${SOURCES})

EOL
fi

# Initialize a new git repository in the DSP directory
cd $OBJECT_NAME~/${OBJECT_NAME}_dsp
git init

# Print a completion message
echo "Done! You can now start developing your pd object in the $OBJECT_NAME~ directory."