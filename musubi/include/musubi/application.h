/// @file
/// @author stuhlmeier
/// @date 22 November 2019

#ifndef MUSUBI_APPLICATION_H
#define MUSUBI_APPLICATION_H

#include "musubi/common.h"
#include "musubi/event_looper.h"
#include "musubi/exception.h"
#include "musubi/input.h"
#include "musubi/rw_lock.h"
#include "musubi/window.h"

#include <functional>
#include <iostream>
#include <thread>
#include <typeindex>
#include <unordered_map>

namespace musubi {
    using namespace std::literals;

    /// @brief The central management instance for a musubi application.
    /// @details
    /// Applications perform window management and handle input polling.
    ///
    /// @ref window "Windows" created (@ref create_window()) by an application are internally registered
    /// and automatically updated every tick. An application instance owns all of the windows it creates.
    ///
    /// To be registrable to an application, a window must have an associated @ref input_poller implementation.
    /// If the window type has a member typedef named `poller_type`, it is used by default;
    /// otherwise, it is possible to manually specify an input poller type as a template parameter to @ref create_window().
    /// Only one instance of an input poller is created for each associated window type,
    /// and queried on every tick.
    ///
    /// @tparam EventLoop The event looper type used as this application's main loop.
    /// The constructor for EventLoop must accept at least one argument;
    /// the first parameter must accept a value of type `std::function<void()>`.
    /// The default event looper is @ref blocking_looper.
    /// @see window
    template<typename EventLoop = blocking_looper>
    class application final {
    public:
        /// @brief Constructs an application, forwarding this application's tick function
        /// and the specified arguments to the constructor of EventLoop.
        /// @tparam LooperArgs a parameter pack to hold the forwarded arguments
        /// @param args the arguments to forward to the constructor of EventLoop
        /// @see application
        template<typename ...LooperArgs>
        explicit application(LooperArgs &&...args)
                : _state(std::make_unique<state>()),
                  looper(EventLoop(std::bind(&application::update, this), std::forward<LooperArgs>(args)...)) {}

        /// @brief Destroys this window and releases its resources.
        /// @details All registered windows are also destroyed.
        ~application() = default;

        /// @details Retrieves this application's event looper.
        /// @return this application's event looper
        /// @see application
        [[nodiscard]] inline EventLoop &get_looper() noexcept { return looper; }

        /// @details Retrieves this application's event looper.
        /// @return this application's event looper
        /// @see application
        [[nodiscard]] inline const EventLoop &get_looper() const noexcept { return looper; }

        /// @brief Retrieves the input state for the specified window.
        /// @param windowId the target window's identifier
        /// @return the input state for the specified window
        [[nodiscard]] inline input_state get_input(window::id_type windowId) const {
            auto lock{_state->input_lock.read_lock()};
            return _state->state.window_states[windowId];
        }

        /// @brief Creates and registers a window, forwarding the specified arguments to the constructor of Window,
        /// and returns a @ref weak_ptr to it.
        /// @details Additionally, a poller of type Poller is registered if one does not already exist.
        /// @tparam Window the window type to create
        /// @tparam Poller the associated @ref input_poller type, `Window::poller_type` by default
        /// @tparam WindowArgs a parameter pack to hold the forwarded arguments
        /// @param args the arguments to forward to the constructor of Window
        /// @return a weak pointer to the newly-created window
        template<typename Window, typename Poller = typename Window::poller_type, typename... WindowArgs>
        [[nodiscard]] inline std::enable_if_t<
                std::is_base_of_v<window, Window> && std::is_base_of_v<input_poller, Poller>,
                std::weak_ptr<Window>
        >
        create_window(WindowArgs &&...args) {
            {
                auto lock{_state->input_lock.write_lock()};
                _state->pollers.emplace(typeid(Poller), std::make_unique<Poller>());
            }

            const auto ptr = std::make_shared<Window>(std::forward<WindowArgs>(args)...);
            const auto id = ptr->get_id();

            {
                auto lock{_state->windows_lock.write_lock()};
                const auto result = _state->windows.emplace(id, std::dynamic_pointer_cast<window>(ptr));
                if (!result.second)
                    throw application_error("A window "s + std::to_string(id) + " is already registered"s);
            }

            return ptr;
        }

    private:
        struct state {
            rw_lock windows_lock;
            std::unordered_map<window::id_type, std::shared_ptr<window>> windows;

            rw_lock input_lock;
            std::unordered_map<std::type_index, std::unique_ptr<input_poller>> pollers;
            application_input_state state;
        };

        void update() {
            auto inputLock{_state->input_lock.write_lock()};

            for (const auto &[id, poller] : _state->pollers) {
                poller->poll(_state->state);
            }

            if (_state->state.global_state.request_close) {
                looper.request_stop();
            }

            auto windowLock{_state->windows_lock.read_lock()};

            for (auto it = _state->windows.begin(); it != _state->windows.end();) {
                const auto id = it->first;
                const auto window = it->second;

                const auto &input = _state->state.window_states[id];
                if (input.request_close) {
                    log_i("application") << "Received close request for window " << id << '\n';
                    it = _state->windows.erase(it);
                    log_i("application") << "Discarded window " << id << '\n';
                } else ++it;

                window->update(input);
            }
        }

        std::unique_ptr<state> _state;
        EventLoop looper;
    };
}

#endif //MUSUBI_APPLICATION_H
