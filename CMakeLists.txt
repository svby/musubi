cmake_minimum_required(VERSION 3.12)
project(libmusubi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(
        musubi_sources
        src/stub.cpp
)

set(
        musubi_headers
        include/stub.h
)

add_library(
        musubi STATIC
        ${musubi_sources}
        ${musubi_headers}
)

target_include_directories(musubi PUBLIC include/)

install(TARGETS musubi ARCHIVE DESTINATION lib/musubi)
install(FILES ${musubi_headers} DESTINATION include/musubi)
