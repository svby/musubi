/// @file
/// @author stuhlmeier
/// @date 7 December 2019

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

    /// @brief An @ref application_error indicating that an error occurred during shader compilation/linking.
    /// @details This exception stores @ref get_shader_log "the error log" of the affected shader or program.
    class shader_error : public application_error {
    public:
        /// @brief Constructs a @ref shader_error from the given error message and shader log.
        /// @details
        /// The final error string is composed of the provided error message and,
        /// if present, the optional shader log contents.
        ///
        /// @param[in] what the error message
        /// @param[in] shaderLog the optional contents of the shader or program log
        explicit shader_error(const std::string &what, std::optional<std::string> shaderLog = nullopt) noexcept;

        /// @brief Retrieves the contents of the associated shader or program log, if present.
        /// @return the optional contents of the associated shader or program log
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
