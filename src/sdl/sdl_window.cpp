//
// Created by stuhlmeier on 11/23/19.
//

#include <sdl/sdl_window.h>
#include <iostream>
#include <cstdint>

namespace {
    const int SDL_UNDEFINED_X = SDL_WINDOWPOS_UNDEFINED; // NOLINT(hicpp-signed-bitwise)
    const int SDL_UNDEFINED_Y = SDL_UNDEFINED_X;

    inline constexpr std::uint32_t windowModeFlags(musubi::window_mode windowMode) {
        switch (windowMode) {
            case musubi::window_mode::windowed:
                return 0;
            case musubi::window_mode::fullscreen:
                return SDL_WINDOW_FULLSCREEN;
            case musubi::window_mode::maximized:
                return SDL_WINDOW_MAXIMIZED;
            case musubi::window_mode::minimized:
                return SDL_WINDOW_MINIMIZED;
            default:
                // TODO exceptions
                return 0;
        }
    }
}

namespace musubi::sdl {
    sdl_window::sdl_window(const start_info &startInfo)
            : wrapped(nullptr), context(nullptr) {
        // TODO exceptions!
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

        wrapped = SDL_CreateWindow(
                startInfo.title.c_str(),
                musubi::numeric_cast<int>(startInfo.x == start_info::UNDEFINED_X ? SDL_UNDEFINED_X : startInfo.x),
                musubi::numeric_cast<int>(startInfo.y == start_info::UNDEFINED_Y ? SDL_UNDEFINED_Y : startInfo.y),
                startInfo.width, startInfo.height,

                SDL_WINDOW_OPENGL // NOLINT(hicpp-signed-bitwise)
                | (startInfo.hidden ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN)
                | (startInfo.undecorated ? SDL_WINDOW_BORDERLESS : 0)
                | (startInfo.resizable ? SDL_WINDOW_RESIZABLE : 0)
                | windowModeFlags(startInfo.window_mode)
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
