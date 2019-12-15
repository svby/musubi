//
// Created by stuhlmeier on 12/13/19.
//

#ifndef MUSUBI_GL_TEXTURES_H
#define MUSUBI_GL_TEXTURES_H

#include "musubi/common.h"
#include "musubi/renderer.h"
#include "musubi/pixmap.h"

#include <epoxy/gl.h>

namespace musubi::gl {
    class texture final {
    public:
        LIBMUSUBI_DELCP(texture)

        texture() noexcept;

        explicit texture(const pixmap &source, GLenum internalFormat = GL_RGBA8);

        texture(texture &&other) noexcept;

        texture &operator=(texture &&other) noexcept;

        ~texture() noexcept;

        GLuint load(const pixmap &source, GLenum internalFormat = GL_RGBA8);

        explicit operator bool() const noexcept;

        operator GLuint() const noexcept; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    private:
        GLuint handle{0};
    };

    class gl_texture_renderer final : public renderer {
    private:
        LIBMUSUBI_PIMPL

    public:
        LIBMUSUBI_DELCP(gl_texture_renderer)

        gl_texture_renderer() noexcept;

        ~gl_texture_renderer() noexcept override;

        void init() override;

        void begin_batch(std::shared_ptr<texture> texture);

        void end_batch(bool resetTransform);

        void batch_draw_texture(GLfloat x, GLfloat y, GLfloat width, GLfloat height);
    };
}

#endif //MUSUBI_GL_TEXTURES_H
