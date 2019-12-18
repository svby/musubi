//
// Created by stuhlmeier on 12/13/19.
//

#ifndef MUSUBI_GL_TEXTURES_H
#define MUSUBI_GL_TEXTURES_H

#include "musubi/common.h"
#include "musubi/renderer.h"
#include "musubi/pixmap.h"

#include <epoxy/gl.h>

#include <memory>

namespace musubi::gl {
    class texture final {
    public:
        LIBMUSUBI_DELCP(texture)

        texture() noexcept;

        explicit texture(const pixmap &source, bool shouldFlip = false, GLenum internalFormat = GL_RGBA8);

        texture(texture &&other) noexcept;

        texture &operator=(texture &&other) noexcept;

        ~texture() noexcept;

        GLuint load(const pixmap &source, bool shouldFlip = false, GLenum internalFormat = GL_RGBA8);

        [[nodiscard]] bool should_flip() const noexcept;

        explicit operator bool() const noexcept;

        operator GLuint() const noexcept; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    private:
        GLuint handle{0};
        bool flip{false};
    };

    struct texture_region final {
    public:
        std::weak_ptr<texture> texture;
        GLfloat u1, v1, u2, v2;

        texture_region(const std::shared_ptr<::musubi::gl::texture> &texture,
                       GLfloat u1, GLfloat v1, GLfloat u2, GLfloat v2) noexcept;

        texture_region(const std::shared_ptr<::musubi::gl::texture> &texture) noexcept;
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

        void batch_draw_region(const texture_region &region, GLfloat x, GLfloat y, GLfloat width, GLfloat height);
    };
}

#endif //MUSUBI_GL_TEXTURES_H
