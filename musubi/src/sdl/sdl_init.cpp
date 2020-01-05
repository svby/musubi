/// @file
/// @author stuhlmeier
/// @date 27 November 2019

#include <musubi/sdl/sdl_init.h>
#include <musubi/sdl/sdl_error.h>

#include <SDL2/SDL.h>

#include <string>

namespace musubi::sdl {
    using namespace std::literals;

    void init() {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            throw sdl_error("Could not initialize SDL: "s + SDL_GetError());
        }
    }

    void dispose() noexcept {
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    }

    scoped_init::scoped_init() { init(); }

    scoped_init::~scoped_init() noexcept { dispose(); }
}
