cmake_minimum_required(VERSION 3.1)
project(demo LANGUAGES C)

aux_source_directory(./src demo_SOURCES)
add_executable(demo ${demo_SOURCES})
