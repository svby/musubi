cmake_minimum_required(VERSION 3.12)
project(libmusubi LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(
        musubi_sources
        src/event_looper.cpp
        src/exception.cpp
        src/sdl/sdl_window.cpp
        src/sdl/sdl_input_poller.cpp
)

set(
        musubi_public_headers
        include/application.h
        include/input.h
        include/window.h
        include/event_looper.h
        include/exception.h
        include/util/common.h
        include/util/rw_lock.h
)

# SDL support
set(
        musubi_sdl_public_headers
        include/sdl/sdl_window.h
        include/sdl/sdl_input_poller.h
        include/sdl/sdl_exception.h
)

set(
        musubi_headers
        ${musubi_public_headers}
        ${musubi_sdl_public_headers}
)

add_library(
        musubi STATIC
        ${musubi_sources}
        ${musubi_headers}
)

find_package(SDL2 REQUIRED)
target_include_directories(musubi PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(musubi ${SDL2_LIBRARIES})

find_package(Epoxy REQUIRED)
target_include_directories(musubi PRIVATE ${Epoxy_INCLUDE_DIRS})
target_link_libraries(musubi ${Epoxy_LIBRARIES})

include_directories(include/)
target_include_directories(musubi PUBLIC include/)

install(TARGETS musubi ARCHIVE DESTINATION lib/musubi)
install(DIRECTORY include/ DESTINATION include/musubi)
