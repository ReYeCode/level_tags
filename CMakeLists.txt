cmake_minimum_required(VERSION 3.27.9)

# The project name doesn't have to strictly match the ID, but it's good practice.
project(level-tags-mod VERSION 1.0.0)

# Find the Geode SDK
find_package(Geode REQUIRED)

# This standard Geode macro reads your mod.json and src/main.cpp to build the mod
setup_geode_mod()