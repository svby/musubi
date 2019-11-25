//
// Created by stuhlmeier on 11/23/19.
//

#include <sdl/sdl_window.h>
#include <iostream>

namespace musubi::sdl {
    sdl_window::sdl_window(const std::string &title, uint32 width, uint32 height)
            : wrapped(nullptr), context(nullptr) {
        // TODO exceptions!
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

        wrapped = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // NOLINT(hicpp-signed-bitwise)
                width, height,
                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        if (wrapped == nullptr) {
            log_e("sdl_window") << "Could not create SDL window: " << SDL_GetError() << '\n';
            return;
        }

        context = SDL_GL_CreateContext(wrapped);
        if (context == nullptr) {
            log_e("sdl_window") << "Could not create GL context: " << SDL_GetError() << '\n';
            return;
        }
    }

    sdl_window::sdl_window(musubi::sdl::sdl_window &&other) noexcept
            : wrapped(std::exchange(other.wrapped, nullptr)),
              context(std::exchange(other.context, nullptr)) {}

    sdl_window &sdl_window::operator=(musubi::sdl::sdl_window &&other) noexcept {
        wrapped = std::exchange(other.wrapped, nullptr);
        context = std::exchange(other.context, nullptr);
        return *this;
    }

    sdl_window::~sdl_window() {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(wrapped);
    }

    sdl_window::id_type sdl_window::get_id() const { return SDL_GetWindowID(wrapped); }

    void sdl_window::make_current() const {
        SDL_GL_MakeCurrent(wrapped, context);
    }

    void sdl_window::flip() const {
        SDL_GL_SwapWindow(wrapped);
    }

}
