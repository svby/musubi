/// @file
/// @author stuhlmeier
/// @date 6 December 2019

#ifndef MUSUBI_GL_COMMON_H
#define MUSUBI_GL_COMMON_H

#include <epoxy/gl.h>

namespace musubi::gl {
    /// Converts an `unsigned int` to a `void *` offset expected by OpenGL functions like `glBufferSubData`.
    /// @param[in] i the buffer offset
    /// @return the buffer offset as a `void *`
    inline GLvoid *get_buffer_offset(GLuint i) { return reinterpret_cast<void *>(i); }
}

#endif //MUSUBI_GL_COMMON_H
