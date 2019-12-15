//
// Created by stuhlmeier on 12/7/19.
//

#ifndef MUSUBI_GL_SHADERS_H
#define MUSUBI_GL_SHADERS_H

#include "musubi/common.h"
#include "musubi/exception.h"

#include <epoxy/gl.h>

#include <cstdint>
#include <optional>
#include <string_view>

namespace musubi::gl {
    using std::nullopt;

    class shader_error : public application_error {
    public:
        explicit shader_error(const std::string &what, std::optional<std::string> shaderLog = nullopt) noexcept;

        [[nodiscard]] const std::optional<std::string> &get_shader_log() const noexcept;

    private:
        std::optional<std::string> shaderLog;
    };

    class shader_program final {
    public:
        LIBMUSUBI_DELCP(shader_program)

        shader_program() noexcept;

        shader_program(std::string_view vertexSource, std::string_view fragmentSource);

        shader_program(shader_program &&other) noexcept;

        shader_program &operator=(shader_program &&other) noexcept;

        ~shader_program() noexcept;

        GLuint link(std::string_view vertexSource, std::string_view fragmentSource);

        explicit operator bool() const noexcept;

        operator GLuint() const noexcept; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    private:
        GLuint handle{0};
    };
}

#endif //MUSUBI_GL_SHADERS_H
