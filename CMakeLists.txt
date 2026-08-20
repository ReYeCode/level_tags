cmake_minimum_required(VERSION 3.25)

# Ensure CMake can locate the Geode SDK directory
if(NOT DEFINED GEODE_SDK)
    set(GEODE_SDK $ENV{GEODE_SDK})
endif()

if(DEFINED GEODE_SDK)
    list(APPEND CMAKE_PREFIX_PATH "${GEODE_SDK}")
endif()

# Find and initialize Geode
find_package(Geode REQUIRED)

# Define project details
project(MyGeodeMod VERSION 1.0.0 LANGUAGES CXX)

# Configure build settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define your mod binary and include source files
setup_geode_mod(
    ${PROJECT_NAME}
    SOURCES
        src/main.cpp
)