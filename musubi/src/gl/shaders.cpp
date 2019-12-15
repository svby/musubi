//
// Created by stuhlmeier on 12/7/19.
//

#include <musubi/gl/shaders.h>

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
    using std::nullopt;

    std::string combineError(const std::string &what, const std::optional<std::string> &shaderLog) noexcept {
        if (shaderLog == nullopt) return what;
        else {
            std::ostringstream error;
            error << what << ":\n" << shaderLog->data();
            return error.str();
        }
    }

    GLuint compileShader(const GLenum shaderType, const std::string_view source) {
        const auto id = glCreateShader(shaderType);
        const auto data = source.data();
        const auto length = static_cast<GLint>(source.length());
        glShaderSource(id, 1, &data, &length);
        glCompileShader(id);

        GLint success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE) {
            std::ostringstream error;
            error << "Error compiling ";
            switch (shaderType) {
                case GL_VERTEX_SHADER:
                    error << "vertex";
                    break;
                case GL_FRAGMENT_SHADER:
                    error << "fragment";
                    break;
                case GL_GEOMETRY_SHADER:
                    error << "geometry";
                    break;
                case GL_COMPUTE_SHADER:
                    error << "compute";
                    break;
                case GL_TESS_CONTROL_SHADER:
                    error << "tessellation control";
                    break;
                case GL_TESS_EVALUATION_SHADER:
                    error << "tessellation evaluation";
                    break;
                default:
                    error << "unknown (" << shaderType << ')';
                    break;
            }
            error << " shader";

            GLint logLength;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

            std::string shaderLog;
            if (logLength == 0) shaderLog = "(no shader log available)";
            else {
                std::unique_ptr<GLchar[]> buffer(new GLchar[logLength]{});
                glGetShaderInfoLog(id, logLength, &logLength, buffer.get());
                shaderLog = std::string(buffer.get(), logLength);
            }

            glDeleteShader(id);

            throw musubi::gl::shader_error(error.str(), shaderLog);
        }

        return id;
    }
}

namespace musubi::gl {
    shader_error::shader_error(const std::string &what, std::optional<std::string> shaderLog) noexcept
            : application_error(combineError(what, shaderLog)), shaderLog(std::move(shaderLog)) {}

    const std::optional<std::string> &shader_error::get_shader_log() const noexcept { return shaderLog; }

    shader_program::shader_program() noexcept = default;

    shader_program::shader_program(const std::string_view vertexSource, const std::string_view fragmentSource) {
        link(vertexSource, fragmentSource);
    }

    shader_program::shader_program(shader_program &&other) noexcept
            : handle(std::exchange(other.handle, 0)) {}

    shader_program &shader_program::operator=(shader_program &&other) noexcept {
        handle = std::exchange(other.handle, 0);
        return *this;
    }

    shader_program::~shader_program() noexcept {
        if (operator bool()) glDeleteProgram(handle);
        handle = 0;
    }

    GLuint shader_program::link(const std::string_view vertexSource, const std::string_view fragmentSource) {
        if (handle != 0) this->~shader_program();

        const auto vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
        const auto fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

        const auto id = glCreateProgram();

        glAttachShader(id, vertex);
        glAttachShader(id, fragment);

        glLinkProgram(id);

        glDetachShader(id, vertex);
        glDetachShader(id, fragment);

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        GLint success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);

        if (success == GL_FALSE) {
            GLint logLength;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

            std::string programLog;
            if (logLength == 0) programLog = "(no program log available)";
            else {
                std::unique_ptr<GLchar[]> buffer(new GLchar[logLength]);
                glGetProgramInfoLog(id, logLength, &logLength, buffer.get());
                programLog = std::string(buffer.get(), logLength);
            }

            glDeleteProgram(id);

            throw shader_error("Error linking shader program", programLog);
        }

        handle = id;
        return id;
    }

    shader_program::operator bool() const noexcept { return handle != 0; }

    shader_program::operator GLuint() const noexcept { return handle; }
}
