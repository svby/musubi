/// @file
/// @author stuhlmeier
/// @date 13 December 2019

#ifndef MUSUBI_RENDERER_H
#define MUSUBI_RENDERER_H

#include "musubi/camera.h"

#include <glm/mat4x4.hpp>

namespace musubi {
    /// @brief A common interface for _renderer objects_.
    /// @details
    /// Renderer objects own a @ref camera (projection and view matrices) and a transformation matrix,
    /// and must be _@ref init "initialized"_ before they are used.
    class renderer {
    protected:
        /// @brief Constructs an uninitialized renderer.
        renderer();

    public:
        /// @brief The camera used for rendering.
        ::musubi::camera camera{};

        /// @brief The transformation matrix used for rendering.
        glm::mat4 transform{1.0f};

        /// @details Destroys this renderer and releases its resources.
        virtual ~renderer();

        /// @details Copy constructor.
        /// @param[in] other the renderer to copy from
        renderer(const renderer &other);

        /// @details Copy assignment operator.
        /// @param[in] other the renderer to copy from
        /// @return this
        renderer &operator=(const renderer &other);

        /// @details Move constructor.
        /// @param[in,out] other the renderer to move from
        renderer(renderer &&other) noexcept;

        /// @details Move assignment operator.
        /// @param[in,out] other the renderer to move from
        /// @return this
        renderer &operator=(renderer &&other) noexcept;

        /// @brief Initializes this renderer for rendering.
        virtual void init() = 0;
    };
}

#endif //MUSUBI_RENDERER_H
