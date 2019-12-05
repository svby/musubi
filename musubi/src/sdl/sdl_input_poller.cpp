//
// Created by stuhlmeier on 11/24/19.
//

#include <sdl/sdl_input_poller.h>

#include <util/common.h>

#include <SDL_events.h>

namespace musubi::sdl {
    void sdl_input_poller::poll(application_input_state &inputState) {
        const std::size_t EVENT_QUEUE_SIZE = 4u;
        static SDL_Event eventQueue[EVENT_QUEUE_SIZE];

        SDL_PumpEvents();

        while (true) {
            switch (const int read = SDL_PeepEvents(
                    eventQueue, EVENT_QUEUE_SIZE,
                    SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT
            )) {
                case 0:
                    return;
                case -1:
                    log_e("sdl_input_poller") << "Could not poll events: " << SDL_GetError() << '\n';
                    return;
                default:
                    for (int i = 0u; i < read; ++i) {
                        const auto &event = eventQueue[i];

                        switch (event.type) {
                            case SDL_QUIT: {
                                inputState.global_state.request_close = true;
                                break;
                            }
                            case SDL_WINDOWEVENT: {
                                switch (event.window.event) {
                                    case SDL_WINDOWEVENT_CLOSE: {
                                        inputState.window_states[event.window.windowID].request_close = true;
                                        break;
                                    }
                                    default: {
                                        // TODO
                                        break;
                                    }
                                }
                                break;
                            }
                            case SDL_MOUSEMOTION: {
                                // TODO
                                break;
                            }
                            default: {
                                log_w("sdl_input_poller") << "Unhandled event type " << event.type << " received\n";
                                break;
                            }
                        }
                    }
                    break;
            }
        }
    }
}
