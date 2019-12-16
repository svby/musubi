//
// Created by stuhlmeier on 12/6/19.
//

#ifndef MUSUBI_GL_COMMON_H
#define MUSUBI_GL_COMMON_H

#include <epoxy/gl.h>

namespace musubi::gl {
    inline GLvoid *get_buffer_offset(GLuint i) { return reinterpret_cast<void *>(i); }
}

#endif //MUSUBI_GL_COMMON_H
