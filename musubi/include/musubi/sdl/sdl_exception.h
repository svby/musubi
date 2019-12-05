//
// Created by stuhlmeier on 11/27/19.
//

#ifndef LIBMUSUBI_SDL_SDL_EXCEPTION_H
#define LIBMUSUBI_SDL_SDL_EXCEPTION_H

#include <exception>
#include <stdexcept>

namespace musubi::sdl {
    class sdl_exception : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
}

#endif //LIBMUSUBI_SDL_SDL_EXCEPTION_H
