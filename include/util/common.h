//
// Created by stuhlmeier on 11/22/19.
//

#ifndef LIBMUSUBI_UTIL_COMMON_H
#define LIBMUSUBI_UTIL_COMMON_H

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <iostream>
#include <typeinfo>
#include <iomanip>

#define LIBMUSUBI_DELCP(t) t(t const &other) = delete; t &operator=(t const &other) = delete;
#define LIBMUSUBI_DELMV(t) t(t &&other) = delete; t &operator=(t &&other) = delete;

namespace musubi {
    using int8 = std::int_fast8_t;
    using int16 = std::int_fast16_t;
    using int32 = std::int_fast32_t;
    using int64 = std::int_fast64_t;

    using uint8 = std::uint_fast8_t;
    using uint16 = std::uint_fast16_t;
    using uint32 = std::uint_fast32_t;
    using uint64 = std::uint_fast64_t;

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

    template<typename To, typename From>
    To numeric_cast(From from) {
        using limits = std::numeric_limits<To>;

        if (from <= limits::max()) return static_cast<To>(from);
        else if (from >= limits::min()) return static_cast<To>(from - limits::min()) + limits::min();
        else {
            std::ostringstream error;
            error << typeid(From).name() << " value " << from
                  << " is out of range of target type " << typeid(To).name();
            throw std::domain_error(error.str());
        }
    }
}

#endif //LIBMUSUBI_UTIL_COMMON_H
