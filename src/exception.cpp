//
// Created by stuhlmeier on 11/27/19.
//

#include <exception.h>

namespace musubi {
    assertion_error::assertion_error() : std::logic_error("assertion failed") {}
}