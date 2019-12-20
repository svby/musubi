//
// Created by stuhlmeier on 11/23/19.
//

#ifndef LIBMUSUBI_SDL_SDL_WINDOW_H
#define LIBMUSUBI_SDL_SDL_WINDOW_H

#include "musubi/sdl/sdl_input_poller.h"
#include "musubi/application.h"
#include "musubi/common.h"
#include "musubi/input.h"
#include "musubi/screen.h"
#include "musubi/window.h"

#include <SDL2/SDL.h>

#include <chrono>
#include <string>
#include <memory>

namespace musubi::sdl {
    class sdl_window : public virtual window {
    public:
        using poller_type = sdl_input_poller;

        LIBMUSUBI_DELCP(sdl_window)

        explicit sdl_window(const musubi::window::start_info &startInfo);

        sdl_window(const musubi::window::start_info &startInfo, std::shared_ptr<screen> initialScreen);

        sdl_window(sdl_window &&other) noexcept;

        sdl_window &operator=(sdl_window &&other) noexcept;

        ~sdl_window() override;

        void set_screen(std::shared_ptr<screen> newScreen) final;

        template<typename Screen, typename... ScreenArgs>
        std::enable_if_t<std::is_base_of_v<screen, Screen>, void>
        set_screen(ScreenArgs &&...args) { set_screen(std::make_shared<Screen>(std::forward<ScreenArgs>(args)...)); }

        void update(const input_state &inputState) override;

        [[nodiscard]] id_type get_id() const override;

        void make_current() const;

        void flip() const;

    private:
        using clock_type = std::chrono::steady_clock;
        using delta_type = std::chrono::duration<float>;

        SDL_Window *wrapped;
        SDL_GLContext context;
        std::shared_ptr<screen> currentScreen;
        std::chrono::time_point<clock_type> lastTime;
    };
}

#endif //LIBMUSUBI_SDL_SDL_WINDOW_H
