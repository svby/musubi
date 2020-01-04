/// @file
/// @author stuhlmeier
/// @date 22 November 2019

#ifndef MUSUBI_UTIL_COMMON_H
#define MUSUBI_UTIL_COMMON_H

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <typeinfo>
#include <experimental/propagate_const>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define LIBMUSUBI_DELCP(t) t(t const &other) = delete; t &operator=(t const &other) = delete;
#define LIBMUSUBI_PIMPL struct impl; std::experimental::propagate_const<::musubi::detail::pimpl_t<impl>> pImpl;
#endif //DOXYGEN_SHOULD_SKIP_THIS

namespace musubi {
    using namespace std::literals;

    using int8 = std::int_fast8_t; ///< @details Signed 8-bit integer type
    using int16 = std::int_fast16_t; ///< @details Signed 16-bit integer type
    using int32 = std::int_fast32_t; ///< @details Signed 32-bit integer type
    using int64 = std::int_fast64_t; ///< @details Signed 64-bit integer type
    using uint8 = std::uint_fast8_t;///< @details Unsigned 8-bit integer type
    using uint16 = std::uint_fast16_t; ///< @details Unsigned 16-bit integer type
    using uint32 = std::uint_fast32_t; ///< @details Unsigned 32-bit integer type
    using uint64 = std::uint_fast64_t; ///< @details Unsigned 64-bit integer type

    /// The value of the numeric constant pi.
    template<typename T> constexpr T pi{3.141592653589793238462643383279502884L};

    /// @brief Creates a 24-bit RGB color from a red, green, and blue value.
    /// @param r, g, b the 8-bit RGB values
    /// @return the color, as a single integer
    constexpr uint32 rgb8(uint8 r, uint8 g, uint8 b) {
        return ((r & 0xFFu) << 16u) | ((g & 0xFFu) << 8u) | (b & 0xFFu);
    }

    /// @brief Creates a 32-bit RGBA color from a red, green, blue, and alpha value.
    /// @param r, g, b, a the 8-bit RGBA values
    /// @return the color, as a single integer
    /// @see rgb8
    constexpr uint32 rgba8(uint8 r, uint8 g, uint8 b, uint8 a = 255u) { return (rgb8(r, g, b) << 8u) | a; }

    namespace detail {
        template<typename Impl> using pimpl_t = std::unique_ptr<Impl>;

        constexpr std::string::size_type SCOPE_LENGTH = 16u;

        constexpr const char DEFAULT_SCOPE[] = "root";
        constexpr const char LOG_PREFIX[] = "musubi";
        constexpr const char LOG_PREFIX_INFO[] = "info    ";
        constexpr const char LOG_PREFIX_WARN[] = "warning:";
        constexpr const char LOG_PREFIX_ERROR[] = "ERROR!  ";

        inline std::ostream &log_base(std::ostream &stream, std::string_view &scope, const char *const &prefix) {
            return stream << LOG_PREFIX << '@'
                          << std::left << std::setw(SCOPE_LENGTH) << scope << std::resetiosflags(std::ios::adjustfield)
                          << ' ' << prefix << ' ';
        }

        inline std::ostream &log_i(std::string_view scope = DEFAULT_SCOPE) {
            return log_base(std::cout, scope, LOG_PREFIX_INFO);
        }

        inline std::ostream &log_w(std::string_view scope = DEFAULT_SCOPE) {
            return log_base(std::cerr, scope, LOG_PREFIX_WARN);
        }

        inline std::ostream &log_e(std::string_view scope = DEFAULT_SCOPE) {
            return log_base(std::cerr, scope, LOG_PREFIX_ERROR);
        }
    }
}

#endif //MUSUBI_UTIL_COMMON_H
