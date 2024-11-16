# src/platform/win32/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(jojWin32Platform STATIC platform_win32.c input_win32.c timer_win32.c)

target_link_directories(
    jojWin32Platform 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)


target_link_libraries(jojWin32Platform PUBLIC jojPlatform User32.lib Gdi32.lib winmm.lib)