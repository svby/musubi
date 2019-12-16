//
// Created by stuhlmeier on 11/22/19.
//

#ifndef LIBMUSUBI_UTIL_COMMON_H
#define LIBMUSUBI_UTIL_COMMON_H

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <typeinfo>

#define LIBMUSUBI_DELCP(t) t(t const &other) = delete; t &operator=(t const &other) = delete;

#define LIBMUSUBI_PIMPL struct impl; ::musubi::pimpl_t<impl> pImpl;

namespace musubi {
    using namespace std::literals;

    using int8 = std::int_fast8_t;
    using int16 = std::int_fast16_t;
    using int32 = std::int_fast32_t;
    using int64 = std::int_fast64_t;

    using uint8 = std::uint_fast8_t;
    using uint16 = std::uint_fast16_t;
    using uint32 = std::uint_fast32_t;
    using uint64 = std::uint_fast64_t;

    template<typename T>
    constexpr T pi{3.141592653589793238462643383279502884L};

    constexpr uint32 rgb8(uint8 r, uint8 g, uint8 b) {
        return ((r & 0xFFu) << 16u) | ((g & 0xFFu) << 8u) | (b & 0xFFu);
    }

    constexpr uint32 rgba8(uint8 r, uint8 g, uint8 b, uint8 a = 255u) { return (rgb8(r, g, b) << 8u) | a; }

    template<typename Impl>
    using pimpl_t = std::unique_ptr<Impl>;

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

#endif //LIBMUSUBI_UTIL_COMMON_H
