# src/renderer/d3d11/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojD3D11Renderer 
    STATIC 
    renderer_d3d11.c
)

target_link_directories(
    jojD3D11Renderer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(jojD3D11Renderer PUBLIC jojRenderer dxgi.lib d3d11.lib D3DCompiler.lib)