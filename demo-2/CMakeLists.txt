cmake_minimum_required(VERSION 3.1)
project(demo LANGUAGES C)

file(WRITE "${CMAKE_BINARY_DIR}/include/platform.h.in" "/*(Auto generated from CMakeList.txt)*/
")

include(CheckIncludeFiles)
configure_file(${CMAKE_BINARY_DIR}/include/platform.h.in ${CMAKE_BINARY_DIR}/include/platform.h)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/include)
aux_source_directory(./src demo_SOURCES)
add_executable(demo ${demo_SOURCES})
