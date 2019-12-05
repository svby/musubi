//
// Created by stuhlmeier on 11/27/19.
//

#include <musubi/sdl/sdl_init.h>
#include <musubi/sdl/sdl_exception.h>

#include <SDL.h>

#include <string>

namespace musubi::sdl {
    using namespace std::literals;

    void init() {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            throw sdl_exception("Could not initialize SDL: "s + SDL_GetError());
        }
    }

    void dispose() noexcept {
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    }

    scoped_init::scoped_init() { init(); }

    scoped_init::~scoped_init() noexcept { dispose(); }
}
