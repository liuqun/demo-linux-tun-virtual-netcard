cmake_minimum_required(VERSION 3.1)
project(demo LANGUAGES C)

file(WRITE "${CMAKE_BINARY_DIR}/include/platform.h.in" "/*(Auto generated from CMakeList.txt)*/
#cmakedefine HAVE_LINUX_IF_TUN_H
#cmakedefine HAVE_LINUX_IF_H

#if !defined(HAVE_LINUX_IF_H) || !defined(HAVE_LINUX_IF_TUN_H)
#error \"Could not find linux/if.h and linux/if_tun.h\"
#endif

#include <linux/if_tun.h>  // The place where TUNSETIFF was defined
#include <linux/if.h>  // The place where IFNAMSIZ was defined
")

include(CheckIncludeFiles)
check_include_files("linux/if_tun.h" HAVE_LINUX_IF_TUN_H)
check_include_files("linux/if.h" HAVE_LINUX_IF_H)
configure_file(${CMAKE_BINARY_DIR}/include/platform.h.in ${CMAKE_BINARY_DIR}/include/platform.h)

include_directories(${PROJECT_BINARY_DIR}/include)
aux_source_directory(./src demo_SOURCES)
add_executable(demo ${demo_SOURCES})
