/// @file
/// @author stuhlmeier
/// @date 7 December 2019

#ifndef MUSUBI_CAMERA_H
#define MUSUBI_CAMERA_H

#include <glm/mat4x4.hpp>

namespace musubi {
    /// @brief A container for a projection and view matrix.
    /// @details
    /// Cameras specify how scenes are rendered; they can be provided to a @ref renderer
    /// to affect the rendering process (viewport size, scaling, perspective, etc.).
    /// @see renderer
    struct camera final {
        glm::mat4 projection{}, ///< The projection matrix.
                view{}; ///< The view matrix.

        /// @brief Constructs a camera with identity projection and view matrices.
        camera() noexcept;

        /// @brief Sets up this camera with an orthographic projection.
        /// @details This is suitable for 2D graphics applications.
        /// @param width, height the viewport size
        /// @return this, for chaining
        camera &set_viewport_ortho(float width, float height) noexcept;

        /// @brief Sets the camera position.
        /// @param x, y the desired position
        /// @return this, for chaining
        camera &set_position(float x, float y) noexcept;

        /// @brief Retrieves the current X position.
        /// @return the current X position
        [[nodiscard]] float get_x() const noexcept;

        /// @brief Retrieves the current Y position.
        /// @return the current Y position
        [[nodiscard]] float get_y() const noexcept;

        /// @brief Retrieves the current viewport width.
        /// @return the current viewport width
        [[nodiscard]] float get_width() const noexcept;

        /// @brief Retrieves the current viewport height.
        /// @return the current viewport height
        [[nodiscard]] float get_height() const noexcept;

    private:
        float storedX{0}, storedY{0}, storedWidth{16}, storedHeight{9};
    };
}

#endif //MUSUBI_CAMERA_H
