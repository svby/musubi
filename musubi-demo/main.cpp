#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#include <epoxy/gl.h>

#include <musubi/application.h>
#include <musubi/sdl/sdl_init.h>
#include <musubi/sdl/sdl_window.h>

using namespace musubi;
using namespace std::chrono;

template<typename T>
const T pi = std::acos(T(-1));

int main() {
    {
        sdl::scoped_init disposer;

        application demo;

        const auto windowPtr = demo.create_window<sdl::sdl_window>(window::start_info{
                .width = 1280,
                .height = 720,
                .title = "Hello",
                .window_mode = window_mode::windowed
        });

        const auto start = high_resolution_clock::now();
        demo.get_looper().add_action([&windowPtr, &start](auto &token) {
            const auto elapsed = duration_cast<duration<float>>(high_resolution_clock::now() - start).count();
            const auto SHIFT = 2.0f / 3.0f * pi<float>;

            if (auto window = windowPtr.lock()) {
                window->make_current();

                const auto r = std::sin(elapsed + 0.0f * SHIFT) / 2.0f + 0.5f;
                const auto g = std::sin(elapsed + 1.0f * SHIFT) / 2.0f + 0.5f;
                const auto b = std::sin(elapsed + 2.0f * SHIFT) / 2.0f + 0.5f;

                glClearColor(r, g, b, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                window->flip();
            }
        });

        demo.get_looper().loop();
    }

    return 0;
}
