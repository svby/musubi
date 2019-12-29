/// @file
/// @author stuhlmeier
/// @date 12 December 2019

#ifndef MUSUBI_GL_PIXMAP_H
#define MUSUBI_GL_PIXMAP_H

#include "musubi/common.h"
#include "musubi/exception.h"

#include <cstddef>
#include <exception>
#include <memory>
#include <vector>
#include <type_traits>

namespace musubi {
    using std::byte;

    namespace detail {
        template<std::size_t bytes>
        using smallest_integral_t = std::conditional_t<
                bytes <= 1u,
                std::uint8_t, std::conditional_t<
                        bytes <= 2u,
                        std::uint16_t, std::conditional_t<
                                bytes <= 4u,
                                std::uint32_t,
                                std::uint64_t
                        >
                >
        >;

        template<std::size_t bytes>
        struct fundamental_pixmap_traits {
            static_assert(bytes <= 8u, "bytes must be within the bounds of fundamental integral types");
            using element_type = smallest_integral_t<bytes>;
            static constexpr std::size_t bytes_per_pixel = bytes;
        };
    }

    /// @brief The internal data format of a pixmap.
    /// @details The pixmap format determines the amount of bytes each pixel and its components need for storage.
    enum class pixmap_format : uint8 {
        r8, ///< 8 bits, single-channel
        rgb8, ///< 24 bits, 3 channels
        rgba8 ///< 32 bits, 4 channels
    };

    /// @brief A common interface for pixmap images.
    /// @details
    /// Implementations are required to use contiguous, uniform storage;
    /// @ref data() should return a contiguous array of uniform element size.
    ///
    /// Pixmaps have a specified @ref pixmap_format and support retrieval of pixel data by position.
    class pixmap {
    public:
        /// @details Destroys this pixmap and releases its resources.
        virtual ~pixmap();

        /// @details Retrieves the width of this pixmap, in pixels.
        /// @return the width of this pixmap in pixels
        [[nodiscard]] virtual uint32 get_width() const = 0;

        /// @details Retrieves the height of this pixmap, in pixels.
        /// @return the height of this pixmap in pixels
        [[nodiscard]] virtual uint32 get_height() const = 0;

        /// @details Retrieves a const pointer to this pixmap's data.
        /// @return a const pointer to this pixmap's data
        [[nodiscard]] virtual const byte *data() const = 0;

        /// @details Retrieves this pixmap's @ref pixmap_format "data format".
        /// @return this pixmap's data format
        [[nodiscard]] virtual pixmap_format get_format() const = 0;
    };

    /// @brief A template providing compile-time storage traits for @ref pixmap_format "pixmap_formats".
    /// @details
    /// Implementations should define the following symbols:
    /// <table>
    /// <caption>Required symbols for pixmap_traits</caption>
    /// <tr><th>Type</th><th>Name</th><th>Usage</th></tr>
    /// <tr>
    ///     <td>`typedef`</td>
    ///     <td>`element_type`</td>
    ///     <td>
    ///         The underlying integral type used by this pixel format.
    ///         Must be sufficiently large to store a single pixel.
    ///     </td>
    /// </tr>
    /// <tr>
    ///     <td>`static size_t`</td>
    ///     <td>`bytes_per_pixel`</td>
    ///     <td>The number of bytes used to store each pixel.</td>
    /// </tr>
    /// </table>
    /// @tparam format the @ref pixmap_format that this trait represents
    template<pixmap_format format /**< the @ref pixmap_format that this trait represents */>
    struct pixmap_traits;

    /// A @ref pixmap_traits implementation for the @ref pixmap_format::r8 format.
    template<> struct pixmap_traits<pixmap_format::r8> : public detail::fundamental_pixmap_traits<1> {};

    /// A @ref pixmap_traits implementation for the @ref pixmap_format::rgb8 format.
    template<> struct pixmap_traits<pixmap_format::rgb8> : public detail::fundamental_pixmap_traits<3> {};

    /// A @ref pixmap_traits implementation for the @ref pixmap_format::rgba8 format.
    template<> struct pixmap_traits<pixmap_format::rgba8> : public detail::fundamental_pixmap_traits<4> {};

    /// @brief A memory-backed @ref pixmap.
    /// @tparam Format this pixmap's data format
    /// @tparam Traits the @ref pixmap_traits for this pixmap's data
    /// @details
    /// A backing buffer for a new, empty pixmap is not allocated until an attempt is made
    /// to actually modify the underlying buffer.
    template<pixmap_format Format, typename Traits = pixmap_traits<Format>>
    class buffer_pixmap final : public pixmap {
    public:
        /// @brief Constructs a buffer_pixmap with the specified size.
        /// @details This does not allocate any memory.
        /// @param[in] width, height the pixmap size
        buffer_pixmap(uint32 width, uint32 height) noexcept
                : initialized(false), width(width), height(height) {}

        /// @brief Constructs a buffer_pixmap with the specified size, copying the specified buffer.
        /// @param[in] width, height the pixmap size
        /// @param[in] buffer the source buffer to load
        buffer_pixmap(uint32 width, uint32 height, const byte *buffer)
                : width(width), height(height),
                  buffer(buffer, buffer + get_buffer_size(width, height)),
                  initialized(true) {}

        /// @brief Constructs a buffer_pixmap with the specified size, copying the specified range.
        /// @tparam InputIterator the range iterator type
        /// @param[in] width, height the pixmap size
        /// @param[in] first, last the data range to copy from
        template<typename InputIterator>
        buffer_pixmap(uint32 width, uint32 height, InputIterator first, InputIterator last)
                : width(width), height(height), buffer(first, last), initialized(true) {}

        /// @details Move constructor; `other` becomes an empty but valid pixmap.
        /// @param[in,out] other the pixmap to move from
        buffer_pixmap(buffer_pixmap &&other) noexcept
                : width(other.width), height(other.height),
                  buffer(std::move(other.buffer)),
                  initialized(other.initialized) {
            other.buffer.clear();
        }

        /// @details Move assignment operator; `other` becomes an empty but valid pixmap.
        /// @param[in,out] other the pixmap to move from
        /// @return this
        buffer_pixmap &operator=(buffer_pixmap &&other) noexcept {
            width = other.width;
            height = other.height;
            buffer = std::move(other.buffer);
            initialized = other.initialized;
            other.buffer.clear();
            return *this;
        }

        /// @copydoc pixmap::~pixmap()
        ~buffer_pixmap() override = default;

        [[nodiscard]] uint32 get_width() const override { return width; }

        [[nodiscard]] uint32 get_height() const override { return height; }

        [[nodiscard]] const byte *data() const override { return buffer.data(); }

        [[nodiscard]] pixmap_format get_format() const override { return Format; }

        /// @details Checks if this pixmap has allocated a backing buffer.
        /// @return whether this pixmap has allocated a backing buffer
        [[nodiscard]] bool is_allocated() const noexcept { return initialized; }

        /// @brief Allocates a backing buffer for this pixmap if it does not have one.
        /// @details Does nothing otherwise.
        /// @see is_allocated()
        void ensure_buffer() {
            if (!is_allocated()) buffer.resize(get_buffer_size(width, height));
            initialized = true;
        }

        /// @details Retrieves a mutable pointer to this pixmap's data.
        /// @return a mutable pointer to this pixmap's data
        [[nodiscard]] byte *data() { return buffer.data(); }

        /// @brief Retrieves the data of a specified pixel.
        /// @param[in] x, y the position of the pixel
        /// @return the value of the specified pixel
        [[nodiscard]] typename Traits::element_type get_pixel(uint32 x, uint32 y) const {
            if (x >= width) {
                throw std::out_of_range(
                        "x out of pixmap range: "s + std::to_string(x) + " >= "s + std::to_string(width)
                );
            } else if (y >= height) {
                throw std::out_of_range(
                        "y out of pixmap range: "s + std::to_string(y) + " >= "s + std::to_string(height)
                );
            }
            // Combine bytes_per_pixel bytes into a single element_type
            typename Traits::element_type result{0u};
            if (!is_allocated()) return result;
            const auto ptr = get_ptr(x, y);
            const auto max{Traits::get_bytes_per_pixel()};
            for (auto i = 0u; i < max; ++i) {
                result |= (static_cast<typename Traits::element_type>(ptr[i]) << ((max - i - 1) * 8));
            }
            return result;
        }

        /// @brief Sets the value of a specified pixel.
        /// @param[in] x, y the position of the pixel
        /// @param[in] value the new value of the pixel
        void set_pixel(uint32 x, uint32 y, typename Traits::element_type value) {
            // Decompose a element_type into bytes_per_pixel bytes
            if (x >= width) {
                throw std::out_of_range(
                        "x out of pixmap range: "s + std::to_string(x) + " >= "s + std::to_string(width)
                );
            } else if (y >= height) {
                throw std::out_of_range(
                        "y out of pixmap range: "s + std::to_string(y) + " >= "s + std::to_string(height)
                );
            }
            ensure_buffer();
            const auto ptr = get_ptr(x, y);
            const auto max{Traits::bytes_per_pixel};
            for (auto i = 0u; i < max; ++i) {
                ptr[i] = static_cast<byte>((value >> ((max - i - 1) * 8)) & 0xFFu);
            }
        }

    private:
        [[nodiscard]] static constexpr std::size_t get_buffer_size(uint32 width, uint32 height) noexcept {
            return width * height * Traits::bytes_per_pixel;
        }

        [[nodiscard]] inline const byte *get_ptr(uint32 x, uint32 y) const {
            if (!is_allocated())
                throw assertion_error("const buffer_pixmap requested byte*, but it has no allocated buffer");
            return &buffer[y * width * Traits::bytes_per_pixel + x * Traits::bytes_per_pixel];
        }

        [[nodiscard]] inline byte *get_ptr(uint32 x, uint32 y) {
            return const_cast<byte *>((const_cast<const buffer_pixmap *>(this)->get_ptr(x, y)));
        }

        [[nodiscard]] inline byte &get_value(uint32 x, uint32 y) {
            ensure_buffer();
            return buffer[y * width + x];
        }

        [[nodiscard]] inline const byte &get_value(uint32 x, uint32 y) const { return get_value(x, y); }

        bool initialized{false};
        std::vector<byte> buffer;
        uint32 width, height;
    };
}

#endif //MUSUBI_GL_PIXMAP_H
