/// @file
/// @author stuhlmeier
/// @date 13 December 2019

#ifndef MUSUBI_GL_TEXTURES_H
#define MUSUBI_GL_TEXTURES_H

#include "musubi/common.h"
#include "musubi/renderer.h"
#include "musubi/pixmap.h"

#include <epoxy/gl.h>

#include <memory>

namespace musubi::gl {
    /// @brief A wrapper for an OpenGL texture name.
    /// @details
    /// This class additionally contains a flag representing whether
    /// the texture should be vertically flipped prior to rendering;
    /// this is usually true for textures loaded from disk.
    ///
    /// Texture objects do **not** store the contents of the buffer/pixmap used to create them
    /// and thus have no internal state.
    ///
    /// Textures are considered "valid" if they contain an OpenGL texture name (see @ref is_valid() const).
    /// Valid textures are guaranteed to point to an existing texture,
    /// as long as the texture name has not manually been deleted.
    class texture final {
    public:
        LIBMUSUBI_DELCP(texture)

        /// @brief Constructs an invalid texture.
        texture() noexcept;

        /// @brief Constructs and loads a texture from a @ref pixmap using the specified internal texture format.
        /// @details Equivalent to constructing an invalid texture and invoking @ref load() on it.
        /// @param[in] source the source pixmap to be loaded by `glTexImage2D` or similar
        /// @param[in] shouldFlip whether the texture should be vertically flipped prior to rendering
        /// @param[in] internalFormat the OpenGL internal image format for the loaded texture
        /// @see load()
        explicit texture(const pixmap &source, bool shouldFlip = false, GLenum internalFormat = GL_RGBA8);

        /// @details Move constructor; `other` becomes invalid.
        /// @param[in,out] other the texture to move from
        texture(texture &&other) noexcept;

        /// @details Move assignment operator; `other` becomes invalid.
        /// @param[in,out] other the texture to move from
        /// @return this
        texture &operator=(texture &&other) noexcept;

        /// @brief Destroys this texture, deleting the associated OpenGL handle if present.
        ~texture() noexcept;

        /// @brief Loads a texture from a @ref pixmap using the specified internal texture format.
        /// @param[in] source the source pixmap to be loaded by `glTexImage2D` or similar
        /// @param[in] shouldFlip whether the texture should be vertically flipped prior to rendering
        /// @param[in] internalFormat the OpenGL internal image format for the loaded texture
        /// @return the newly-created texture name
        GLuint load(const pixmap &source, bool shouldFlip = false, GLenum internalFormat = GL_RGBA8);

        /// @brief Checks if this texture should be vertically flipped prior to rendering.
        /// @details If this is not a valid texture, this function returns `false`.
        /// @return whether this texture should be vertically flipped prior to rendering
        /// @see texture
        [[nodiscard]] bool should_flip() const noexcept;

        /// @brief Checks if this texture is valid (i.e. contains an OpenGL texture name.)
        /// @return whether this texture is valid
        [[nodiscard]] bool is_valid() const noexcept;

        /// @copydoc is_valid() const
        /// @see is_valid() const
        explicit operator bool() const noexcept(noexcept(is_valid()));

        /// @brief Retrieves the stored OpenGL texture name, or 0 if this is not a valid texture.
        /// @return a valid texture name, or 0
        [[nodiscard]] GLuint get_name() const noexcept;

        /// @copydoc get_name() const
        /// @see get_name() const
        operator GLuint() const noexcept(noexcept(get_name())); // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

    private:
        GLuint handle{0};
        bool flip{false};
    };

    /// @brief A rectangular region of a @ref texture.
    /// @details
    /// This class contains a weak pointer to a @ref texture along with two pairs of UV texture coordinates
    /// that define the offset and extent of the texture region.
    /// @see texture
    struct texture_region final {
    public:
        /// A weak pointer to the associated @ref texture.
        std::weak_ptr<texture> texture;

        ///@{ @name Texture coordinates
        /// The two pairs of UV texture coordinates that define this texture region.
        GLfloat u1, ///< @details The U coordinate of point 1.
                v1, ///< @details The V coordinate of point 1.
                u2, ///< @details The U coordinate of point 2.
                v2; ///< @details The V coordinate of point 2.
        ///@}

        /// @brief Constructs a texture region from the specified texture and texture coordinates.
        /// @param[in] texture a pointer to the associated @ref texture
        /// @param[in] u1, v1, u2, v2 the two pairs of UV texture coordinates that define this texture region
        texture_region(std::weak_ptr<::musubi::gl::texture> texture,
                       GLfloat u1, GLfloat v1, GLfloat u2, GLfloat v2) noexcept;

        /// @brief Constructs a texture region spanning the entire specified texture.
        /// @param[in] texture a pointer to the associated @ref texture
        explicit texture_region(std::weak_ptr<::musubi::gl::texture> texture) noexcept;
    };

    /// @brief A @ref renderer for @ref texture "textures" and @ref texture_region "texture regions".
    /// @details
    /// This renderer processes _batches_ of draw operations.
    /// All draw operations performed in the same batch use the same texture.
    class gl_texture_renderer final : public renderer {
    private:
        LIBMUSUBI_PIMPL

    public:
        LIBMUSUBI_DELCP(gl_texture_renderer)

        /// @copydoc renderer()
        gl_texture_renderer() noexcept;

        /// @copydoc ~renderer()
        ~gl_texture_renderer() noexcept override;

        /// @copydoc renderer::init()
        /// @details This compiles the default texture shader program and initializes a vertex array.
        void init() override;

        /// @brief Begins a texture batch.
        /// @details Prepares an empty triangle mesh for rendering and sets the batch texture.
        /// @param[in] texture the batch texture
        /// @throw illegal_state_error if there is already an active batch
        /// @throw invalid_argument if the specified texture pointer is empty,
        /// or its content is not a valid texture
        void begin_batch(std::shared_ptr<texture> texture);

        /// @brief Finalizes and renders the current batch.
        /// @details
        /// Loads the current triangle mesh into a vertex buffer, draws it and disposes it,
        /// then clears the batch texture.
        /// @param[in] resetTransform whether to reset the transformation matrix after finishing the batch
        /// @throw illegal_state_error if there is no active batch
        void end_batch(bool resetTransform);

        /// @brief Fully draws the current batch texture.
        /// @param[in] x, y the position at which to draw the texture
        /// @param[in] width, height the desired size of the texture
        /// @throw illegal_state_error if there is no active batch
        void batch_draw_texture(GLfloat x, GLfloat y, GLfloat width, GLfloat height);

        /// @brief Draws a region of the current batch texture.
        /// @param[in] region the texture region to draw
        /// @param[in] x, y the position at which to draw the texture region
        /// @param[in] width,height the dimensions of the texture region
        /// @throw illegal_state_error if there is no active batch
        /// @throw invalid_argument if the specified texture region refers to a deleted texture,
        /// or if it does not refer to the current batch texture
        void batch_draw_region(const texture_region &region, GLfloat x, GLfloat y, GLfloat width, GLfloat height);
    };
}

#endif //MUSUBI_GL_TEXTURES_H
