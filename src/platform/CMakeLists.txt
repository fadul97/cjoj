# src/renderer/CMakeLists.txt
cmake_minimum_required(VERSION 3.8)
project(joj)

add_library(jojPlatform STATIC platform.c)

# Specify include directories for jojPlatform
target_include_directories(
    jojPlatform
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)