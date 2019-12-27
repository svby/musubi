/// @file
/// @author stuhlmeier
/// @date 27 November 2019

#ifndef MUSUBI_SDL_SDL_ERROR_H
#define MUSUBI_SDL_SDL_ERROR_H

#include "musubi/exception.h"

#include <exception>
#include <stdexcept>

namespace musubi::sdl {
    /// @brief An @ref application_error indicating that an SDL library call has failed.
    class sdl_error : public application_error {
        using application_error::application_error;
    };
}

#endif //MUSUBI_SDL_SDL_ERROR_H
