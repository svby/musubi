/// @file
/// @author stuhlmeier
/// @date 27 November 2019

#include <musubi/exception.h>

namespace musubi {
    assertion_error::assertion_error() : std::logic_error("assertion failed") {}
}
