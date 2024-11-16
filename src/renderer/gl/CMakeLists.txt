# src/renderer/gl/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

if (WIN32)
    add_library(jojGLRenderer STATIC joj_gl_win32.c)
endif()

target_link_directories(
    jojGLRenderer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# Find the OpenGL package
find_package(OpenGL REQUIRED)

# Include the OpenGL headers directory
target_include_directories(jojGLRenderer PRIVATE ${OPENGL_INCLUDE_DIR})

# Link OpenGL and JojPlatform to JojGraphicsGL
target_link_libraries(jojGLRenderer PUBLIC jojRenderer ${OPENGL_LIBRARY})