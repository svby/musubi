cmake_minimum_required(VERSION 3.13)
project(musubi_demo LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(musubi_demo main.cpp)

target_link_libraries(musubi_demo /usr/local/lib/musubi/libmusubi.a)

find_package(SDL2 REQUIRED)
target_include_directories(musubi_demo PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(musubi_demo ${SDL2_LIBRARIES})

find_package(Epoxy REQUIRED)
target_include_directories(musubi_demo PRIVATE ${Epoxy_INCLUDE_DIRS})
target_link_libraries(musubi_demo ${Epoxy_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(musubi_demo Threads::Threads)
