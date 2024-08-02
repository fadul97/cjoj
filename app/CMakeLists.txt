# CMakeList.txt : CMake project for jojApp, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(jojApp main.c )

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET jojApp PROPERTY C_STANDARD 11)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)

target_link_directories(jojApp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src/)
target_link_libraries(jojApp PUBLIC joj)

# TODO: Add tests and install targets if needed.
