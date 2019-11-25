//
// Created by stuhlmeier on 11/24/19.
//

#include <event_looper.h>

namespace musubi {
    void event_looper::stop_token::request_stop() volatile { stop = true; }

    bool event_looper::stop_token::is_stop_requested() volatile const { return stop; }

    void event_looper::request_stop() { token.request_stop(); }

    bool event_looper::is_stop_requested() const { return token.is_stop_requested(); }

    volatile event_looper::stop_token &event_looper::get_stop_token() { return token; }

    volatile const event_looper::stop_token &event_looper::get_stop_token() const { return token; }

    blocking_looper::blocking_looper(std::function<void()> updater)
            : updater(std::move(updater)) {}

    blocking_looper &blocking_looper::add_action(std::function<void(volatile stop_token &)> functor) {
        functors.emplace_back(std::move(functor));
        return *this;
    }

    blocking_looper::~blocking_looper() = default;

    void blocking_looper::loop() {
        while (!is_stop_requested()) {
            updater();
            for (const auto &functor : functors) {
                functor(token);
                if (is_stop_requested()) return;
            }
        }
    }
}
