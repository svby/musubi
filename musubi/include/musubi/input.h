/// @file
/// @author stuhlmeier
/// @date 23 November 2019

#ifndef MUSUBI_INPUT_H
#define MUSUBI_INPUT_H

#include "musubi/common.h"
#include "musubi/window.h"

#include <unordered_map>

namespace musubi {
    /// @brief A @ref window's input state.
    /// @details Input states are updated by an @ref application's @ref input_poller "input pollers" on every tick.
    /// @see application, input_poller
    struct input_state final {
        /// @brief Whether this window has received a close request.
        /// @details If this flag is set, the @ref application will destroy this window on the next tick.
        bool request_close{false};
    };

    /// @brief An @ref application's aggregate input state.
    /// @details Contains a map of all @ref window input states.
    /// @see input_state
    struct application_input_state final {
        /// The global input state.
        input_state global_state;
        /// The map of window input states.
        std::unordered_map<window::id_type, input_state> window_states;
    };

    /// @brief A common interface for @ref window _input pollers_.
    /// @details
    /// Implementations should update the @ref application_input_state::window_states "window input states"
    /// for all applicable windows; for example, a poller for Framework XYZ should update the input state
    /// for all windows created or managed by that framework.
    /// Whether they should provide events for windows that are not managed by an @ref application is unspecified.
    /// @see application
    class input_poller {
    public:
        /// @details Destroys this poller and releases its resources.
        virtual ~input_poller() = 0;

        /// @brief Polls input from the system, updating the application's input state.
        /// @param inputState the application's global input state
        virtual void poll(application_input_state &inputState) = 0;
    };
}

#endif //MUSUBI_INPUT_H
