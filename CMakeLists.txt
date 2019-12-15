cmake_minimum_required(VERSION 3.12)
project(musubi LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

add_subdirectory(musubi)
add_subdirectory(musubi-demo)
