//
// Created by stuhlmeier on 12/13/19.
//

#include <musubi/gl/textures.h>

#include <musubi/common.h>
#include <musubi/gl/common.h>
#include <musubi/gl/shaders.h>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <utility>
#include <vector>

namespace {
    using namespace std::literals;

    constexpr glm::mat4 IDENTITY{glm::identity<glm::mat4>()};

    constexpr GLenum getGlFormat(musubi::pixmap_format format) {
        switch (format) {
            case musubi::pixmap_format::r8:
                return GL_RED;
            case musubi::pixmap_format::rgb8:
                return GL_RGB;
            case musubi::pixmap_format::rgba8:
                return GL_RGBA;
            default:
                throw musubi::assertion_error(
                        "Cannot construct image format GLenum from unknown pixmap_format "s +
                        std::to_string(static_cast<std::underlying_type_t<musubi::pixmap_format>>(format))
                );
        }
    }
}

namespace musubi::gl {
    texture::texture() noexcept = default;

    texture::texture(const pixmap &source, bool shouldFlip, const GLenum internalFormat) {
        load(source, shouldFlip, internalFormat);
    }

    texture::texture(texture &&other) noexcept
            : handle(std::exchange(other.handle, 0)) {}

    texture &texture::operator=(texture &&other) noexcept {
        handle = std::exchange(other.handle, 0);
        return *this;
    }

    texture::~texture() noexcept {
        if (operator bool()) {
            glDeleteTextures(1, &handle);
            musubi::log_i("texture") << "Deleted texture " << handle << '\n';
        }
        flip = false;
        handle = 0;
    }

    bool texture::should_flip() const noexcept { return flip; }

    GLuint texture::load(const pixmap &source, bool shouldFlip, const GLenum internalFormat) {
        if (handle != 0) this->~texture();
        flip = shouldFlip;

        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(
                GL_TEXTURE_2D, 0, internalFormat, source.get_width(), source.get_height(), 0,
                getGlFormat(source.get_format()),
                GL_UNSIGNED_BYTE,
                source.data()
        );
//        glGenerateMipmap(GL_TEXTURE_2D);

        return handle;
    }

    texture::operator bool() const noexcept { return handle != 0; }

    texture::operator GLuint() const noexcept { return handle; }

    texture_region::texture_region(const std::shared_ptr<::musubi::gl::texture> &texture,
                                   GLfloat u1, GLfloat v1, GLfloat u2, GLfloat v2) noexcept
            : texture(texture), u1(u1), v1(v1), u2(u2), v2(v2) {}

    texture_region::texture_region(const std::shared_ptr<::musubi::gl::texture> &texture) noexcept
            : texture(texture), u1(0), v1(0), u2(1), v2(1) {}

    struct gl_texture_renderer::impl {
        shader_program shader{};
        // Cached locations
        GLint modelMatrixUniform{-1}, viewMatrixUniform{-1}, projectionMatrixUniform{-1};
        GLint textureUniform{-1};

        GLuint vao{0};

        GLuint count{0};
        std::vector<GLfloat> vertices{};
        std::vector<GLfloat> texCoords{};

        bool drawing{false};
        std::shared_ptr<texture> currentTexture;

        LIBMUSUBI_DELCP(impl)

        impl() = default;

        void init() {
            shader.link(
                    "#version 330\n"
                    "\n"
                    "uniform mat4 mP;\n"
                    "uniform mat4 mV;\n"
                    "uniform mat4 mM;\n"
                    "layout (location = 0) in vec2 vPosition;\n"
                    "layout (location = 1) in vec2 uv;\n"
                    "\n"
                    "out vec2 f_uv;\n"
                    "\n"
                    "void main() {\n"
                    "    gl_Position = mP * mV * mM * vec4(vPosition, 0, 1);\n"
                    "    f_uv = uv;\n"
                    "}",

                    "#version 330\n"
                    "\n"
                    "uniform sampler2D u_texture;\n"
                    "\n"
                    "in vec2 f_uv;\n"
                    "\n"
                    "out vec4 color;\n"
                    "void main() {\n"
                    "    color = texture2D(u_texture, f_uv);\n"
                    "}"
            );

            textureUniform = glGetUniformLocation(shader, "u_texture");
            modelMatrixUniform = glGetUniformLocation(shader, "mM");
            viewMatrixUniform = glGetUniformLocation(shader, "mV");
            projectionMatrixUniform = glGetUniformLocation(shader, "mP");

            glGenVertexArrays(1, &vao);
        }

        ~impl() { glDeleteVertexArrays(1, &vao); }

        void begin_batch(std::shared_ptr<texture> texture) {
            if (drawing) throw illegal_state_error("Cannot call begin_batch twice, renderer is already drawing");
            if (!texture) throw std::invalid_argument("Cannot begin texture batch; specified texture pointer is empty");
            if (!*texture) throw std::invalid_argument("Cannot begin texture batch; specified texture is invalid");
            currentTexture = std::move(texture);
            drawing = true;
        }

        void end_batch(const gl_texture_renderer &parent) {
            if (!drawing) throw illegal_state_error("Cannot end texture batch, begin has not yet been called");
            flush(parent);
            currentTexture.reset();
            drawing = false;
        }

        void flush(const gl_texture_renderer &parent) {
            GLuint vbo{0};
            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * 2 * sizeof(GLfloat), vertices.data());
            glBufferSubData(
                    GL_ARRAY_BUFFER,
                    count * 2 * sizeof(GLfloat), count * 2 * sizeof(GLfloat),
                    texCoords.data()
            );

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, get_buffer_offset(count * 2 * sizeof(GLfloat)));

            glUseProgram(shader);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, *currentTexture);

            glUniform1i(textureUniform, 0);
            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(parent.transform));
            glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(parent.camera.view));
            glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(parent.camera.projection));

            glDrawArrays(GL_TRIANGLES, 0, count);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            glDeleteBuffers(1, &vbo);

            glUseProgram(0);
            glBindVertexArray(0);

            count = 0;
            vertices.clear();
            texCoords.clear();
        }

        std::shared_ptr<texture> check_current_texture(const std::weak_ptr<texture> &weakPtr) {
            const auto ptr = weakPtr.lock();

            if (!ptr) throw std::invalid_argument("Specified texture_region refers to a deleted texture");
            if (!currentTexture) throw assertion_error("Current texture pointer is empty");
            if (ptr != currentTexture)
                throw std::invalid_argument("Cannot add operation to batch; "
                                            "the current batch texture and the specified texture_region's texture differ");

            return ptr;
        }

        void draw_region_impl(bool flip,
                              GLfloat x, GLfloat y, GLfloat width, GLfloat height,
                              GLfloat u1, GLfloat v1, GLfloat u2, GLfloat v2) {
            // Triangle 1
            vertices.push_back(x);
            vertices.push_back(y + height);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(x + width);
            vertices.push_back(y + height);

            // Triangle 2
            vertices.push_back(x + width);
            vertices.push_back(y);
            vertices.push_back(x + width);
            vertices.push_back(y + height);
            vertices.push_back(x);
            vertices.push_back(y);

            if (!flip) {
                // Triangle 1
                texCoords.push_back(u1);
                texCoords.push_back(v2);
                texCoords.push_back(u1);
                texCoords.push_back(v1);
                texCoords.push_back(u2);
                texCoords.push_back(v2);

                // Triangle 2
                texCoords.push_back(u2);
                texCoords.push_back(v1);
                texCoords.push_back(u2);
                texCoords.push_back(v2);
                texCoords.push_back(u1);
                texCoords.push_back(v1);
            } else {
                // Triangle 1
                texCoords.push_back(u1);
                texCoords.push_back(1 - v2);
                texCoords.push_back(u1);
                texCoords.push_back(1 - v1);
                texCoords.push_back(u2);
                texCoords.push_back(1 - v2);

                // Triangle 2
                texCoords.push_back(u2);
                texCoords.push_back(1 - v1);
                texCoords.push_back(u2);
                texCoords.push_back(1 - v2);
                texCoords.push_back(u1);
                texCoords.push_back(1 - v1);
            }

            count += 6;
        }

        void batch_draw_texture(GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
            if (!drawing) throw illegal_state_error("Cannot add draw operation; batch has not been begun");
            draw_region_impl(currentTexture->should_flip(), x, y, width, height, 0, 0, 1, 1);
        }

        void batch_draw_region(const texture_region &region,
                               GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
            if (!drawing) throw illegal_state_error("Cannot add draw operation; batch has not been begun");

            const auto texture = check_current_texture(region.texture);
            draw_region_impl(texture->should_flip(), x, y, width, height, region.u1, region.v1, region.u2, region.v2);
        }
    };

    gl_texture_renderer::gl_texture_renderer() noexcept : pImpl(std::make_unique<impl>()) {}

    gl_texture_renderer::~gl_texture_renderer() noexcept = default;

    void gl_texture_renderer::init() { pImpl->init(); }

    void gl_texture_renderer::begin_batch(std::shared_ptr<texture> texture) { pImpl->begin_batch(std::move(texture)); }

    void gl_texture_renderer::end_batch(bool resetTransform) {
        pImpl->end_batch(*this);
        if (resetTransform) transform = IDENTITY;
    }

    void gl_texture_renderer::batch_draw_texture(GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
        pImpl->batch_draw_texture(x, y, width, height);
    }

    void gl_texture_renderer::batch_draw_region(const texture_region &region,
                                                GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
        pImpl->batch_draw_region(region, x, y, width, height);
    }
}
