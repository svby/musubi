/// @file
/// @author stuhlmeier
/// @date 27 November 2019

#ifndef MUSUBI_SDL_SDL_INIT_H
#define MUSUBI_SDL_SDL_INIT_H

namespace musubi::sdl {
    /// @brief Initializes the SDL subsystems used by musubi.
    /// @details This internally uses `SDL_InitSubSystem`, so initializations are ref-counted.
    void init();

    /// @brief Quits the SDL subsystems used by musubi.
    /// @details This internally uses `SDL_QuitSubSystem`.
    void dispose() noexcept;

    /// @brief A scope guard that calls @ref init() on construction and @ref dispose() on destruction.
    struct scoped_init final {
        /// @brief Calls @ref init().
        scoped_init();

        /// @brief Calls @ref dispose().
        ~scoped_init() noexcept;
    };
}

#endif //MUSUBI_SDL_SDL_INIT_H
