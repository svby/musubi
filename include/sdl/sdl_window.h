//
// Created by stuhlmeier on 11/23/19.
//

#ifndef LIBMUSUBI_SDL_WINDOW_H
#define LIBMUSUBI_SDL_WINDOW_H

#include "sdl_input_poller.h"
#include "../util/common.h"
#include "../window.h"
#include "../application.h"

#include <SDL.h>

#include <string>

namespace musubi::sdl {
    class sdl_window : public virtual window {
    public:
        using poller_type = sdl_input_poller;

        LIBMUSUBI_DELCP(sdl_window)

        explicit sdl_window(const musubi::window::start_info &startInfo);

        sdl_window(sdl_window &&other) noexcept;

        sdl_window &operator=(sdl_window &&other) noexcept;

        ~sdl_window() override;

        [[nodiscard]] id_type get_id() const override;

        void make_current() const;

        void flip() const;

    private:
        SDL_Window *wrapped;
        SDL_GLContext context;
    };
}

#endif //LIBMUSUBI_SDL_WINDOW_H
