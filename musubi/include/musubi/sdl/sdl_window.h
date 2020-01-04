/// @file
/// @author stuhlmeier
/// @date 23 November 2019

#ifndef MUSUBI_SDL_SDL_WINDOW_H
#define MUSUBI_SDL_SDL_WINDOW_H

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
    /// @brief An SDL window with an OpenGL context.
    class sdl_window : public window {
    public:
        /// The associated @ref input_poller implementation.
        /// @see sdl_input_poller
        using poller_type = sdl_input_poller;

        LIBMUSUBI_DELCP(sdl_window)

        /// Constructs an SDL window from the specified @ref window::start_info "start_info".
        /// @param startInfo initialization parameters
        explicit sdl_window(const start_info &startInfo);

        /// Constructs an SDL window from the specified @ref window::start_info "start_info" and initial @ref screen.
        /// @param startInfo initialization parameters
        /// @param initialScreen the initial screen
        /// @see set_screen()
        sdl_window(const start_info &startInfo, std::unique_ptr<screen> initialScreen);

        /// @details Move constructor; `other` becomes invalid.
        /// @param[in,out] other the window to move from
        sdl_window(sdl_window &&other) noexcept;

        /// @details Move assignment operator; `other` becomes invalid.
        /// @param[in,out] other the window to move from
        /// @return this
        sdl_window &operator=(sdl_window &&other) noexcept;

        /// @brief Destroys this wrapper and the associated SDL window.
        ~sdl_window() override;

        /// @brief Sets this window's current screen.
        /// @details The old screen instance is destroyed.
        /// @param newScreen the new screen
        void set_screen(std::unique_ptr<screen> newScreen) final;

        /// @brief Constructs a @ref screen by forwarding the specified arguments to the constructor of Screen,
        /// and makes it the current screen.
        /// @details The old screen instance is destroyed.
        /// @tparam Screen the screen type
        /// @tparam ScreenArgs a parameter pack to hold the forwarded arguments
        /// @param args the arguments to forward to the constructor of Screen
        template<typename Screen, typename... ScreenArgs>
        std::enable_if_t<std::is_base_of_v<screen, Screen>, void>
        set_screen(ScreenArgs &&...args) { set_screen(std::make_unique<Screen>(std::forward<ScreenArgs>(args)...)); }

        void update(const input_state &inputState) override;

        [[nodiscard]] id_type get_id() const override;

        /// @brief Makes this window's OpenGL context current.
        void make_current() const;

        /// @brief Swaps this window's buffers.
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

#endif //MUSUBI_SDL_SDL_WINDOW_H
