//
// Created by stuhlmeier on 12/6/19.
//

#include <musubi/gl/shapes.h>

#include <musubi/gl/common.h>
#include <musubi/gl/shaders.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace {
    constexpr glm::mat4 IDENTITY{glm::identity<glm::mat4>()};
}

namespace musubi::gl {
    struct gl_shape_renderer::impl {
        shader_program shader{};
        // Cached locations
        GLint modelMatrixUniform{-1}, viewMatrixUniform{-1}, projectionMatrixUniform{-1};

        GLuint vao{0};

        GLuint count{0};
        std::vector<GLfloat> vertices{};
        std::vector<GLfloat> colors{};

        bool drawing{false};

        LIBMUSUBI_DELCP(impl)

        impl() = default;

        void init() {
            shader.link(
                    "#version 330\n"
                    "\n"
                    "uniform mat4 mP;\n"
                    "uniform mat4 mV;\n"
                    "uniform mat4 mM;\n"
                    "layout (location = 0) in vec4 vPosition;\n"
                    "layout (location = 1) in vec4 vColor;\n"
                    "\n"
                    "out vec4 color;\n"
                    "\n"
                    "void main() {\n"
                    "    color = vColor;\n"
                    "    gl_Position = mP * mV * mM * vPosition;\n"
                    "}",

                    "#version 330\n"
                    "\n"
                    "out vec4 fColor;\n"
                    "in vec4 color;\n"
                    "\n"
                    "void main() {\n"
                    "    fColor = color;\n"
                    "}"
            );

            modelMatrixUniform = glGetUniformLocation(shader, "mM");
            viewMatrixUniform = glGetUniformLocation(shader, "mV");
            projectionMatrixUniform = glGetUniformLocation(shader, "mP");

            glGenVertexArrays(1, &vao);
        }

        ~impl() { glDeleteVertexArrays(1, &vao); }

        void begin_batch() {
            if (drawing) throw illegal_state_error("Cannot call begin_batch twice, renderer is already drawing");
            drawing = true;
        }

        void end_batch(const gl_shape_renderer &parent) {
            if (!drawing) throw illegal_state_error("Cannot end shape batch, begin has not yet been called");
            flush(parent);
            drawing = false;
        }

        void flush(const gl_shape_renderer &parent) {
            GLuint vbo{0};
            glBindVertexArray(vao);
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferData(GL_ARRAY_BUFFER, count * 7 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * 3 * sizeof(GLfloat), vertices.data());
            glBufferSubData(GL_ARRAY_BUFFER, count * 3 * sizeof(GLfloat), count * 4 * sizeof(GLfloat), colors.data());

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, get_buffer_offset(count * 3 * sizeof(GLfloat)));

            glUseProgram(shader);

            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(parent.transform));
            glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(parent.camera.view));
            glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, glm::value_ptr(parent.camera.projection));

            glDrawArrays(GL_LINES, 0, count);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            glDeleteBuffers(1, &vbo);

            glUseProgram(0);
            glBindVertexArray(0);

            count = 0;
            vertices.clear();
            colors.clear();
        }

        void draw_line_impl(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, const glm::vec4 &lineColor) {
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(0.0f);
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(0.0f);
            colors.push_back(lineColor.r);
            colors.push_back(lineColor.g);
            colors.push_back(lineColor.b);
            colors.push_back(lineColor.a);
            colors.push_back(lineColor.r);
            colors.push_back(lineColor.g);
            colors.push_back(lineColor.b);
            colors.push_back(lineColor.a);
            count += 2;
        }

        void batch_draw_line(const gl_shape_renderer &parent, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
            if (!drawing) throw illegal_state_error("Cannot add draw operation; batch has not been begun");
            draw_line_impl(x1, y1, x2, y2, parent.color);
        }

        void batch_draw_rectangle(const gl_shape_renderer &parent, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
            if (!drawing) throw illegal_state_error("Cannot add draw operation; batch has not been begun");

            draw_line_impl(x, y, x + w, y, parent.color);
            draw_line_impl(x + w, y, x + w, y + h, parent.color);
            draw_line_impl(x + w, y + h, x, y + h, parent.color);
            draw_line_impl(x, y + h, x, y, parent.color);
        }

        void batch_draw_circle(const gl_shape_renderer &parent, GLfloat x, GLfloat y, GLfloat r, uint32 segments) {
            if (!drawing) throw illegal_state_error("Cannot add draw operation; batch has not been begun");

            if (segments < 3u) {
                throw std::invalid_argument("Circle segment count must be >= 3, was: "s + std::to_string(segments));
            }

            GLfloat angle = 0;
            GLfloat step = 2 * pi<GLfloat> / segments;
            float lastX{r * std::cos(-step) + x}, lastY{r * std::sin(-step) + y};
            for (GLuint i = 0; i < segments; angle += step, ++i) {
                float currentX = r * std::cos(angle) + x;
                float currentY = r * std::sin(angle) + y;
                draw_line_impl(lastX, lastY, currentX, currentY, parent.color);
                lastX = currentX;
                lastY = currentY;
            }
        }
    };

    gl_shape_renderer::gl_shape_renderer() noexcept : pImpl(std::make_unique<impl>()) {}

    gl_shape_renderer::~gl_shape_renderer() noexcept = default;

    void gl_shape_renderer::init() { pImpl->init(); }

    void gl_shape_renderer::begin_batch() { pImpl->begin_batch(); }

    void gl_shape_renderer::end_batch(bool resetTransform) {
        pImpl->end_batch(*this);
        if (resetTransform) transform = IDENTITY;
    }

    void gl_shape_renderer::batch_draw_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
        pImpl->batch_draw_line(*this, x1, y1, x2, y2);
    }

    void gl_shape_renderer::batch_draw_rectangle(GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
        pImpl->batch_draw_rectangle(*this, x, y, w, h);
    }

    void gl_shape_renderer::batch_draw_circle(GLfloat x, GLfloat y, GLfloat r, uint32 segments) {
        pImpl->batch_draw_circle(*this, x, y, r, segments);
    }
}
