//
// Created by stuhlmeier on 12/6/19.
//

#ifndef MUSUBI_GL_TRIANGLE_H
#define MUSUBI_GL_TRIANGLE_H

#include "musubi/camera.h"
#include "musubi/common.h"
#include "musubi/renderer.h"

#include <epoxy/gl.h>
#include <glm/mat4x4.hpp>

namespace musubi::gl {
    class gl_shape_renderer final : public renderer {
    private:
        LIBMUSUBI_PIMPL

    public:
        glm::vec4 color{1, 0, 0, 0};

        LIBMUSUBI_DELCP(gl_shape_renderer)

        gl_shape_renderer() noexcept;

        ~gl_shape_renderer() noexcept override;

        void init() override;

        void begin_batch();

        void end_batch(bool resetTransform);

        void batch_draw_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

        void batch_draw_rectangle(GLfloat x, GLfloat y, GLfloat w, GLfloat h);

        void batch_draw_circle(GLfloat x, GLfloat y, GLfloat r, uint32 segments = 20);
    };
}

#endif //MUSUBI_GL_TRIANGLE_H
