# - Try to find libepoxy
# Once done, this will define
#
#  Epoxy_FOUND - system has epoxy
#  Epoxy_INCLUDE_DIRS - the epoxy include directories
#  Epoxy_LIBRARIES - link these to use epoxy

include(LibFindMacros)

libfind_pkg_check_modules(Epoxy_PKGCONF epoxy)

find_path(
        Epoxy_INCLUDE_DIR
        NAMES epoxy/gl.h
        HINTS ${Epoxy_PKGCONF_INCLUDE_DIRS}
)

find_library(
        Epoxy_LIBRARY
        NAMES epoxy
        HINTS ${Epoxy_PKGCONF_LIBRARY_DIRS}
)

set(Epoxy_PROCESS_INCLUDES Epoxy_INCLUDE_DIR)
set(Epoxy_PROCESS_LIBS Epoxy_LIBRARY)
set(Epoxy_VERSION ${Epoxy_PKGCONF_VERSION})

libfind_process(Epoxy)
