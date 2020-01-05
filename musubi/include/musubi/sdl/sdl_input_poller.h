/// @file
/// @author stuhlmeier
/// @date 24 November 2019

#ifndef MUSUBI_SDL_SDL_INPUT_POLLER_H
#define MUSUBI_SDL_SDL_INPUT_POLLER_H

#include "musubi/input.h"

namespace musubi::sdl {
    /// @brief An @ref input_poller for SDL windows.
    /// @details This poller populates the input map with the events from all SDL windows,
    /// even if they are not managed by an @ref application.
    class sdl_input_poller : public input_poller {
    public:
        void poll(application_input_state &inputState) override;
    };
}

#endif //MUSUBI_SDL_SDL_INPUT_POLLER_H
