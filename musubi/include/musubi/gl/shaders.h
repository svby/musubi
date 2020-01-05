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

    /// @brief A wrapper for an OpenGL shader program.
    /// @details
    /// Shader objects do **not** store the GLSL source code used to create them
    /// and thus have no internal state.
    ///
    /// Shaders are considered "valid" if they contain an OpenGL shader program handle (see @ref is_valid() const).
    /// Valid shaders are guaranteed to point to an existing shader program,
    /// as long as the shader program has not manually been deleted.
    class shader_program final {
    public:
        LIBMUSUBI_DELCP(shader_program)

        /// @brief Constructs an invalid shader program.
        shader_program() noexcept;

        /// @brief Constructs, compiles and links a shader program from the specified vertex and fragment shader sources.
        /// @details Equivalent to constructing an invalid shader program and invoking @ref link() on it.
        /// @param[in] vertexSource the vertex shader source
        /// @param[in] fragmentSource the fragment shader source
        /// @see link()
        shader_program(std::string_view vertexSource, std::string_view fragmentSource);

        /// @details Move constructor; `other` becomes invalid.
        /// @param[in,out] other the shader program to move from
        shader_program(shader_program &&other) noexcept;

        /// @details Move assignment operator; `other` becomes invalid.
        /// @param[in,out] other the shader program to move from
        /// @return this
        shader_program &operator=(shader_program &&other) noexcept;

        /// @brief Destroys this shader program, deleting the associated OpenGL handle if present.
        ~shader_program() noexcept;

        /// @brief Compiles and links a shader program from the specified vertex and fragment shader sources.
        /// @param[in] vertexSource the vertex shader source
        /// @param[in] fragmentSource the fragment shader source
        /// @return the newly-created shader program handle
        GLuint link(std::string_view vertexSource, std::string_view fragmentSource);

        /// @brief Checks if this shader program is valid (i.e. contains an OpenGL shader program handle.)
        /// @return whether this shader program is valid
        [[nodiscard]] bool is_valid() const noexcept;

        /// @copydoc is_valid() const
        /// @see is_valid() const
        explicit operator bool() const noexcept(noexcept(is_valid()));

        /// @brief Retrieves the stored OpenGL shader program handle, or 0 if this is not a valid shader program.
        /// @return a valid shader program handle, or 0
        [[nodiscard]] GLuint get_handle() const noexcept;

        /// @copydoc get_handle() const
        /// @see get_handle() const
        operator GLuint() const noexcept(noexcept(get_handle())); // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    private:
        GLuint handle{0};
    };
}

#endif //MUSUBI_GL_SHADERS_H
