cmake_minimum_required(VERSION 3.13)

# Force fmt into header-only mode BEFORE CPM/Geode loads to fix Android NDK consteval errors
set(FMT_HEADER_ONLY ON CACHE BOOL "" FORCE)
set(FMT_OS OFF CACHE BOOL "" FORCE)

# Include CPM if present in your repository
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")
    include(cmake/CPM.cmake)
endif()

# If using CPM for fmt, enforce modern 10.2.1 tag
if(COMMAND CPMAddPackage)
    CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 10.2.1
        OPTIONS
            "FMT_HEADER_ONLY ON"
            "FMT_DOC OFF"
            "FMT_TEST OFF"
    )
endif()

# Project definition
project(Level_Tags VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Load Geode SDK
find_package(Geode REQUIRED)

# Include all C++ files inside your src folder
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Geode build target
setup_geode_mod(
    ${PROJECT_NAME}
    SOURCES ${SOURCES}
)