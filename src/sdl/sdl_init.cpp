//
// Created by stuhlmeier on 11/27/19.
//

#include <sdl/sdl_init.h>
#include <SDL.h>

namespace musubi::sdl {
    void init() noexcept {
        SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    }

    void dispose() noexcept {
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    }

    scoped_init::scoped_init() noexcept { init(); }

    scoped_init::~scoped_init() noexcept { dispose(); }
}
