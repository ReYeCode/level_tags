cmake_minimum_required(VERSION 3.13)

# Locate Geode SDK from environment variable if not passed directly
if (NOT DEFINED GEODE_SDK)
    if (DEFINED ENV{GEODE_SDK})
        set(GEODE_SDK $ENV{GEODE_SDK})
    else()
        message(FATAL_ERROR "GEODE_SDK environment variable is not set!")
    endif()
endif()

list(APPEND CMAKE_PREFIX_PATH ${GEODE_SDK})

project(LevelTagSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Geode REQUIRED)

setup_geode_mod(
    ID "YallCode.level_tags"
)

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
)