# src/renderer/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(jojPlatform STATIC platform.c)

# Specify include directories for jojPlatform
target_include_directories(
    jojPlatform
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)

if(WIN32)
    add_subdirectory(win32)

    target_link_directories(
        jojPlatform
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/win32/
    )

    target_link_libraries(jojPlatform PUBLIC jojWin32Platform)
endif()