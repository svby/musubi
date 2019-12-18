//
// Created by stuhlmeier on 12/12/19.
//

#ifndef MUSUBI_GL_PIXMAP_H
#define MUSUBI_GL_PIXMAP_H

#include "musubi/common.h"
#include "musubi/exception.h"

#include <cstddef>
#include <exception>
#include <memory>
#include <vector>

namespace musubi {
    using std::byte;

    enum class pixmap_format : uint8 {
        rgb8, rgba8
    };

    class pixmap {
    public:
        virtual ~pixmap();

        [[nodiscard]] virtual uint32 get_width() const = 0;

        [[nodiscard]] virtual uint32 get_height() const = 0;

        [[nodiscard]] virtual const byte *data() const = 0;

        [[nodiscard]] virtual const byte &operator[](std::size_t index) const = 0;

        [[nodiscard]] virtual pixmap_format get_format() const = 0;
    };

    template<pixmap_format format>
    struct pixmap_traits;

    template<>
    struct pixmap_traits<pixmap_format::rgb8> {
        using element_type = uint32;

        static constexpr uint8 get_bytes_per_pixel() { return 3; }
    };

    template<>
    struct pixmap_traits<pixmap_format::rgba8> {
        using element_type = uint32;

        static constexpr uint8 get_bytes_per_pixel() { return 4; }
    };

    template<pixmap_format Format, typename Traits = pixmap_traits<Format>>
    class buffer_pixmap final : public pixmap {
    public:
        buffer_pixmap(uint32 width, uint32 height) noexcept
                : initialized(false), width(width), height(height) {}

        buffer_pixmap(uint32 width, uint32 height, const byte *buffer)
                : width(width), height(height),
                  buffer(buffer, buffer + get_buffer_size(width, height)),
                  initialized(true) {}

        template<typename Iter>
        buffer_pixmap(uint32 width, uint32 height, Iter first, Iter last)
                : width(width), height(height), buffer(first, last), initialized(true) {}

        buffer_pixmap(buffer_pixmap &&other) noexcept
                : width(other.width), height(other.height),
                  buffer(std::move(other.buffer)),
                  initialized(other.initialized) {}

        buffer_pixmap &operator=(buffer_pixmap &&other) noexcept {
            width = other.width;
            height = other.height;
            buffer = std::move(other.buffer);
            initialized = other.initialized;
        }

        ~buffer_pixmap() override = default;

        [[nodiscard]] uint32 get_width() const override { return width; }

        [[nodiscard]] uint32 get_height() const override { return height; }

        [[nodiscard]] const byte *data() const override { return buffer.data(); }

        [[nodiscard]] const byte &operator[](std::size_t index) const override { return buffer[index]; }

        [[nodiscard]] pixmap_format get_format() const override { return Format; }

        [[nodiscard]] bool is_allocated() const noexcept { return initialized; }

        void ensure_buffer() {
            if (!is_allocated()) buffer.resize(get_buffer_size(width, height));
            initialized = true;
        }

        [[nodiscard]] byte *data() { return buffer.data(); }

        [[nodiscard]] byte &operator[](std::size_t index) { return buffer[index]; }

        [[nodiscard]] typename Traits::element_type get_pixel(uint32 x, uint32 y) const {
            // Combine bytes_per_pixel bytes into a single element_type
            if (x >= width) {
                throw std::out_of_range(
                        "x out of pixmap range: "s + std::to_string(x) + " >= "s + std::to_string(width)
                );
            } else if (y >= height) {
                throw std::out_of_range(
                        "y out of pixmap range: "s + std::to_string(y) + " >= "s + std::to_string(height)
                );
            }
            typename Traits::element_type result{0u};
            if (!is_allocated()) return result;
            const auto ptr = get_ptr(x, y);
            const auto max{Traits::get_bytes_per_pixel()};
            for (auto i = 0u; i < max; ++i) {
                result |= (static_cast<typename Traits::element_type>(ptr[i]) << ((max - i - 1) * 8));
            }
            return result;
        }

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
            const auto max{Traits::get_bytes_per_pixel()};
            for (auto i = 0u; i < max; ++i) {
                ptr[i] = static_cast<byte>((value >> ((max - i - 1) * 8)) & 0xFFu);
            }
        }

    private:
        [[nodiscard]] static constexpr std::size_t get_buffer_size(uint32 width, uint32 height) noexcept {
            return width * height * Traits::get_bytes_per_pixel();
        }

        [[nodiscard]] inline const byte *get_ptr(uint32 x, uint32 y) const {
            if (!is_allocated())
                throw assertion_error("const buffer_pixmap requested byte*, but it has no allocated buffer");
            return &buffer[y * width * Traits::get_bytes_per_pixel() + x * Traits::get_bytes_per_pixel()];
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
