//
// Created by stuhlmeier on 12/6/19.
//

#ifndef MUSUBI_GL_COMMON_H
#define MUSUBI_GL_COMMON_H

namespace musubi::gl {
    inline constexpr char *get_buffer_offset(GLuint i) { return static_cast<char *>(nullptr) + i; }
}

#endif //MUSUBI_GL_COMMON_H
