cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

project(musubi_demo LANGUAGES CXX)

add_executable(musubi_demo main.cpp)

target_compile_features(musubi_demo PRIVATE cxx_std_17)
#target_compile_options(musubi_demo PRIVATE -Wall -Wextra -pedantic)

add_dependencies(musubi_demo musubi)
get_target_property(musubi_INCLUDE_DIRS musubi INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(musubi_demo PRIVATE ${musubi_INCLUDE_DIRS})
target_link_libraries(musubi_demo musubi)

find_package(SDL2 REQUIRED)
target_link_libraries(musubi SDL2::SDL2)

find_package(Epoxy REQUIRED)
target_include_directories(musubi_demo PRIVATE ${Epoxy_INCLUDE_DIRS})
target_link_libraries(musubi_demo ${Epoxy_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(musubi_demo Threads::Threads)

file(GLOB_RECURSE musubi_demo_pack_files assets/**/pack.json)

add_custom_target(
        musubi_demo_assets
        DEPENDS ${musubi_demo_pack_files}
        COMMAND ${CMAKE_SOURCE_DIR}/tools/mpack.py -v -r ${CMAKE_CURRENT_SOURCE_DIR}/assets/
)

add_dependencies(musubi_demo musubi_demo_assets)
