cmake_minimum_required(VERSION 3.12)
set(CMAKE_VERBOSE_MAKEFILE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

project(libmusubi LANGUAGES CXX)

set(
        musubi_sources
        src/event_looper.cpp
        src/exception.cpp
        src/sdl/sdl_window.cpp
        src/sdl/sdl_input_poller.cpp
        src/sdl/sdl_init.cpp
)

set(
        musubi_public_headers
        include/musubi/application.h
        include/musubi/input.h
        include/musubi/window.h
        include/musubi/event_looper.h
        include/musubi/exception.h
)

set(
        musubi_private_headers
        include/musubi/common.h
        include/musubi/rw_lock.h
)

# SDL support
set(
        musubi_sdl_public_headers
        include/musubi/sdl/sdl_init.h
        include/musubi/sdl/sdl_window.h
        include/musubi/sdl/sdl_input_poller.h
        include/musubi/sdl/sdl_exception.h
)

set(
        musubi_headers
        ${musubi_public_headers}
        ${musubi_private_headers}
        ${musubi_sdl_public_headers}
)

add_library(
        musubi STATIC
        ${musubi_sources}
        ${musubi_headers}
)
target_compile_features(musubi PRIVATE cxx_std_17)

find_package(SDL2 REQUIRED)
target_include_directories(musubi PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(musubi ${SDL2_LIBRARIES})

find_package(Epoxy REQUIRED)
target_include_directories(musubi PRIVATE ${Epoxy_INCLUDE_DIRS})
target_link_libraries(musubi ${Epoxy_LIBRARIES})

target_include_directories(musubi PUBLIC include/)
target_include_directories(musubi PRIVATE src/)

install(TARGETS musubi ARCHIVE DESTINATION lib/musubi)
install(DIRECTORY include/musubi DESTINATION include)
