/// @file
/// @author stuhlmeier
/// @date 7 December 2019

#include <musubi/gl/shaders.h>

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
    using namespace std::literals;
    using namespace musubi::detail;
    using std::nullopt;

    std::string combineError(const std::string &what, const std::optional<std::string> &shaderLog) noexcept {
        if (shaderLog == nullopt) return what;
        else {
            std::ostringstream error;
            error << what << ":\n" << shaderLog->data();
            return error.str();
        }
    }

    std::string getShaderTypeName(const GLenum shaderType) {
        switch (shaderType) {
            case GL_VERTEX_SHADER:
                return "vertex";
            case GL_FRAGMENT_SHADER:
                return "fragment";
            case GL_GEOMETRY_SHADER:
                return "geometry";
            case GL_COMPUTE_SHADER:
                return "compute";
            case GL_TESS_CONTROL_SHADER:
                return "tessellation control";
            case GL_TESS_EVALUATION_SHADER:
                return "tessellation evaluation";
            default:
                return "unknown ("s + std::to_string(shaderType) + ")"s;
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
            error << "Error compiling " << getShaderTypeName(shaderType) << " shader";

            GLint logLength;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

            std::string shaderLog;
            if (logLength == 0) shaderLog = "(no shader log available)";
            else {
                auto buffer{std::make_unique<GLchar[]>(logLength)};
                glGetShaderInfoLog(id, logLength, &logLength, buffer.get());
                shaderLog = std::string(buffer.get(), logLength);
            }

            glDeleteShader(id);

            throw musubi::gl::shader_error(error.str(), shaderLog);
        }

        log_i("compileShader") << "Successfully compiled "
                                       << getShaderTypeName(shaderType) << " shader " << id << '\n';

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
        if (operator bool()) {
            glDeleteProgram(handle);
            log_i("shader_program") << "Deleted shader program " << handle << '\n';
        }
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
                auto buffer{std::make_unique<GLchar[]>(logLength)};
                glGetProgramInfoLog(id, logLength, &logLength, buffer.get());
                programLog = std::string(buffer.get(), logLength);
            }

            glDeleteProgram(id);

            throw shader_error("Error linking shader program", programLog);
        }

        log_i("shader_program") << "Successfully linked shader program " << id << '\n';

        handle = id;
        return id;
    }

    bool shader_program::is_valid() const noexcept { return handle != 0; }

    shader_program::operator bool() const noexcept(noexcept(is_valid())) { return is_valid(); }

    GLuint shader_program::get_handle() const noexcept { return handle; }

    shader_program::operator GLuint() const noexcept(noexcept(get_handle())) { return get_handle(); }
}
