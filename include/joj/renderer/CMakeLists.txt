# include/joj/renderer/CMakeLists.txt
cmake_minimum_required(VERSION 3.8)
project(joj)

if(WIN32)
	add_subdirectory(gl)
endif()