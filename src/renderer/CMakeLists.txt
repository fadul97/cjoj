# src/renderer/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(jojRenderer STATIC renderer.c)

# Specify include directories for jojRenderer
target_include_directories(
    jojRenderer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)

if(WIN32)
    # add_subdirectory(gl)

    target_link_directories(
        jojRenderer 
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/../platform/
        # ${CMAKE_CURRENT_SOURCE_DIR}/gl/
    )

    target_link_libraries(jojRenderer PUBLIC jojPlatform )# jojGLRenderer)
endif()