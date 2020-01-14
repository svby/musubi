/// @file
/// @author stuhlmeier
/// @date 27 November 2019

#include <musubi/exception.h>

#include <sstream>

namespace musubi {
    assertion_error::assertion_error() : std::logic_error("assertion failed") {}

    asset_load_error asset_load_error::no_buffer(std::string_view itemName) noexcept {
        std::ostringstream error;
        error << "Could not load asset '";
        error << itemName;
        error << "'; item has no loaded buffer";
        return asset_load_error(error.str());
    }
}
