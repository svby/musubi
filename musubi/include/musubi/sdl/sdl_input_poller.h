//
// Created by stuhlmeier on 11/24/19.
//

#ifndef LIBMUSUBI_SDL_SDL_INPUT_POLLER_H
#define LIBMUSUBI_SDL_SDL_INPUT_POLLER_H

#include "musubi/input.h"

namespace musubi::sdl {
    class sdl_input_poller : public virtual input_poller {
    public:
        void poll(application_input_state &inputState) override;
    };
}

#endif //LIBMUSUBI_SDL_SDL_INPUT_POLLER_H
