/// @file
/// This is a non-includable file which provides Doxygen comments for musubi's namespaces.
/// @author stuhlmeier
/// @date 25 December 2019

#error "This file is present for Doxygen documentation only and should not be included."

/// @brief Contains all of musubi's utilities.
/// @details Extra tools and abstractions for various platforms can be found
/// in children of this namespace (e.g. @ref musubi::gl, @ref musubi::sdl, etc.)
namespace musubi {
    /// Utilities for working with OpenGL graphics.
    namespace gl {}

    /// Utilities for working with SDL windows and input.
    namespace sdl {}
}
