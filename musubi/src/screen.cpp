//
// Created by stuhlmeier on 12/19/19.
//

#include <musubi/screen.h>

namespace musubi {
    screen::~screen() = default;

    basic_screen::~basic_screen() = default;

    void basic_screen::on_attached(window *window) {
        currentWindow = window;
    }

    void basic_screen::on_update(float) {}

    void basic_screen::on_detached(window *window) {
        if (window == currentWindow) currentWindow = nullptr;
    }

    window *basic_screen::get_current_window() const { return currentWindow; }
}
