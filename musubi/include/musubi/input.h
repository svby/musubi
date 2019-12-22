//
// Created by stuhlmeier on 11/23/19.
//

#ifndef MUSUBI_INPUT_H
#define MUSUBI_INPUT_H

#include "musubi/common.h"
#include "musubi/window.h"

#include <unordered_map>

namespace musubi {
    struct input_state final {
        bool request_close{false};
    };

    struct application_input_state final {
        input_state global_state;
        std::unordered_map<window::id_type, input_state> window_states;
    };

    class input_poller {
    public:
        virtual ~input_poller() = default;

        virtual void poll(application_input_state &inputState) = 0;
    };
}

#endif //MUSUBI_INPUT_H
