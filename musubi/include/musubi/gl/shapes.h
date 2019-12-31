/// @file
/// @author stuhlmeier
/// @date 6 December 2019

#ifndef MUSUBI_GL_SHAPES_H
#define MUSUBI_GL_SHAPES_H

#include "musubi/camera.h"
#include "musubi/common.h"
#include "musubi/renderer.h"

#include <epoxy/gl.h>
#include <glm/mat4x4.hpp>

namespace musubi::gl {
    /// @brief A @ref renderer for line-based polygons.
    /// @details
    /// This renderer processes _batches_ of draw operations.
    class gl_shape_renderer final : public renderer {
    private:
        LIBMUSUBI_PIMPL

    public:
        /// @brief The current color to use for drawing polygons.
        glm::vec4 color{1, 0, 0, 0};

        LIBMUSUBI_DELCP(gl_shape_renderer)

        /// @copydoc renderer()
        gl_shape_renderer() noexcept;

        /// @copydoc ~renderer()
        ~gl_shape_renderer() noexcept override;

        /// @copydoc renderer::init()
        /// @details This compiles the default shape shader program and initializes a vertex array.
        void init() override;

        /// @brief Begins a shape batch.
        /// @details Prepares an empty triangle mesh for rendering.
        /// @throw illegal_state_error if there is already an active batch
        void begin_batch();

        /// @brief Finalizes and renders the current batch.
        /// @details
        /// Loads the current triangle mesh into a vertex buffer, draws it and disposes it.
        /// @param[in] resetTransform whether to reset the transformation matrix after finishing the batch
        /// @throw illegal_state_error if there is no active batch
        void end_batch(bool resetTransform);

        /// @brief Draws a line with the specified start and end positions.
        /// @param[in] x1, y1, x2, y2 the two points that define the line
        /// @throw illegal_state_error if there is no active batch
        void batch_draw_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);

        /// @brief Draws a rectangle with the specified position and dimensions.
        /// @param[in] x, y the position of the rectangle
        /// @param[in] w, h the dimensions of the rectangle
        /// @throw illegal_state_error if there is no active batch
        void batch_draw_rectangle(GLfloat x, GLfloat y, GLfloat w, GLfloat h);

        /// @brief Draws a circle with the specified center position, radius, and number of segments.
        /// @param[in] x, y the center position of the circle
        /// @param[in] r the radius of the circle
        /// @param[in] segments the number of segments to draw
        /// @throw illegal_state_error if there is no active batch
        void batch_draw_circle(GLfloat x, GLfloat y, GLfloat r, uint32 segments = 20);
    };
}

#endif //MUSUBI_GL_SHAPES_H
