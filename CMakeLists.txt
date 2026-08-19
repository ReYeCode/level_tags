cmake_minimum_required(VERSION 3.13)

# Force fmt into header-only mode to prevent Android NDK consteval errors
set(FMT_HEADER_ONLY ON CACHE BOOL "" FORCE)
set(FMT_OS OFF CACHE BOOL "" FORCE)

# Tell CMake where to find the Geode SDK path set by GitHub Actions
if(DEFINED ENV{GEODE_SDK})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{GEODE_SDK}")
endif()

project(YourModName VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include CPM if present in your repository
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")
    include(cmake/CPM.cmake)
endif()

# Load Geode SDK
find_package(Geode REQUIRED)

# Collect all C++ source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Configure Geode mod target
setup_geode_mod(${PROJECT_NAME} SOURCES ${SOURCES})