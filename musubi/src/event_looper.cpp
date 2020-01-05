/// @file
/// @author stuhlmeier
/// @date 3 January 2020

#include <musubi/event_looper.h>

namespace musubi {
    void blocking_looper::stop_token::request_stop() { stop = true; }

    bool blocking_looper::stop_token::is_stop_requested() const { return stop; }

    blocking_looper::stop_token::stop_token(volatile bool &stop) : stop(stop) {}

    blocking_looper::blocking_looper(std::function<void()> tickFunction)
            : stop(false), updater(std::move(tickFunction)) {}

    blocking_looper &blocking_looper::add_action(std::function<action_type> function) {
        actions.emplace_back(std::move(function));
        return *this;
    }

    void blocking_looper::request_stop() { stop = true; }

    bool blocking_looper::is_stop_requested() const { return stop; }

    void blocking_looper::loop() {
        while (!is_stop_requested()) {
            updater();
            for (const auto &action : actions) {
                stop_token token(stop);
                action(token);
                if (is_stop_requested()) return;
            }
        }
    }
}
