//
// Created by stuhlmeier on 11/22/19.
//

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

    template<typename EventLoop = blocking_looper>
    class application {
    public:
        struct state {
            rw_lock windowsLock;
            std::unordered_map<window::id_type, std::shared_ptr<window>> windowMap;

            rw_lock inputLock;
            std::unordered_map<std::type_index, std::unique_ptr<input_poller>> pollerMap;
            application_input_state inputState;
        };

        template<typename ...LooperArgs>
        explicit application(LooperArgs &&...args)
                : _state(std::make_shared<state>()),
                  looper(EventLoop(std::bind(&application::update, this), std::forward<LooperArgs>(args)...)) {}

        ~application() = default;

        [[nodiscard]] inline EventLoop &get_looper() noexcept { return looper; }

        [[nodiscard]] inline const EventLoop &get_looper() const noexcept { return looper; }

        [[nodiscard]] inline input_state get_input(window::id_type windowId) const {
            auto lock{_state->inputLock.read_lock()};
            return _state->inputState.window_states[windowId];
        }

        template<typename Window, typename Poller = typename Window::poller_type, typename... WindowArgs>
        [[nodiscard]] inline std::enable_if_t<
                std::is_base_of_v<window, Window> && std::is_base_of_v<input_poller, Poller>,
                std::weak_ptr<Window>
        >
        create_window(WindowArgs &&...args) {
            {
                auto lock{_state->inputLock.write_lock()};
                _state->pollerMap.emplace(typeid(Poller), std::make_unique<Poller>());
            }

            const auto ptr = std::make_shared<Window>(std::forward<WindowArgs>(args)...);
            const auto id = ptr->get_id();

            {
                auto lock{_state->windowsLock.write_lock()};
                const auto result = _state->windowMap.emplace(id, std::dynamic_pointer_cast<window>(ptr));
                if (!result.second)
                    throw application_error("A window "s + std::to_string(id) + " is already registered"s);
            }

            return ptr;
        }

    protected:
        void update() {
            auto inputLock{_state->inputLock.write_lock()};

            for (const auto &[id, poller] : _state->pollerMap) {
                poller->poll(_state->inputState);
            }

            if (_state->inputState.global_state.request_close) {
                looper.request_stop();
            }

            auto windowLock{_state->windowsLock.read_lock()};

            for (auto it = _state->windowMap.begin(); it != _state->windowMap.end();) {
                const auto id = it->first;
                const auto window = it->second;

                const auto &input = _state->inputState.window_states[id];
                if (input.request_close) {
                    log_i("application") << "Received close request for window " << id << '\n';
                    it = _state->windowMap.erase(it);
                    log_i("application") << "Discarded window " << id << '\n';
                } else ++it;

                window->update(input);
            }
        }

    private:
        std::shared_ptr<state> _state;
        EventLoop looper;
    };
}

#endif //MUSUBI_APPLICATION_H
