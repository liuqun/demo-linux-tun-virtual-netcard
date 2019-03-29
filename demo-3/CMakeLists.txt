cmake_minimum_required(VERSION 3.1)
project(demo LANGUAGES C)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBOPENCONNECT REQUIRED openconnect)
if(NOT LIBOPENCONNECT_FOUND)
  message(FATAL_ERROR "Please install libopenconnect-dev (or openconnect-devel)...")
endif()
find_library(LIBOPENCONNECT_LIBRARIES
  NAMES openconnect
  HINTS ${LIBOPENCONNECT_LIBDIR} ${LIBOPENCONNECT_LIBRARY_DIRS}
)

file(WRITE "${CMAKE_BINARY_DIR}/include/platform.h.in" "/*(Auto generated from CMakeList.txt)*/
#pragma once
")

include(CheckIncludeFiles)
configure_file(${CMAKE_BINARY_DIR}/include/platform.h.in ${CMAKE_BINARY_DIR}/include/platform.h)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/include)
aux_source_directory(./src demo_SOURCES)
add_executable(demo ${demo_SOURCES})
set_property(TARGET demo PROPERTY C_STANDARD 11)
target_include_directories(demo PRIVATE ${LIBOPENCONNECT_INCLUSE_DIRS})
target_link_libraries(demo ${LIBOPENCONNECT_LIBRARIES})
