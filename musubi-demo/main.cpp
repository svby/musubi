#include <musubi/application.h>
#include <musubi/gl/shaders.h>
#include <musubi/gl/shapes.h>
#include <musubi/sdl/sdl_init.h>
#include <musubi/sdl/sdl_window.h>

#include <epoxy/gl.h>
#include <glm/ext/matrix_transform.hpp>

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

using namespace musubi;
using namespace std::chrono;
using glm::mat4, glm::vec3, glm::vec4;
using glm::identity, glm::rotate, glm::translate;

int main() {
    {
        sdl::scoped_init disposer;

        application demo;

        const auto windowPtr = demo.create_window<sdl::sdl_window>(window::start_info{
                .width = 1280,
                .height = 720,
                .title = "musubi_demo",
                .window_mode = window_mode::windowed
        });

        gl::gl_shape_renderer renderer;
        renderer.init();

        renderer.camera
                .set_viewport_ortho(1280, 720)
                .set_position(0, 0);

        const auto start = high_resolution_clock::now();
        demo.get_looper().add_action([&](auto &token) {
            const auto elapsed = duration_cast<duration<float>>(high_resolution_clock::now() - start).count();
            const auto SHIFT = 2.0f / 3.0f * pi<float>;

            if (auto window = windowPtr.lock()) {
                window->make_current();

                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                renderer.begin();
                const auto x = 500 * std::sin(elapsed);
                const auto y = 220 * std::cos(2 * pi<float> * elapsed);
                const auto hl = 50.0f;
                renderer.color = vec4{1, 0, 0, 1};
                renderer.transform =
                        rotate(translate(identity<mat4>(), vec3{x, y, 0}), 4 * elapsed, vec3{0, 0, 1});
                renderer.draw_line(-hl, -hl, -hl, hl);
                renderer.draw_line(-hl, hl, hl, hl);
                renderer.draw_line(hl, hl, hl, -hl);
                renderer.draw_line(hl, -hl, -hl, -hl);
                renderer.end(true);

                renderer.begin();
                renderer.color = vec4{
                        std::sin(elapsed + 0.0f * SHIFT) / 2.0f + 0.5f,
                        std::sin(elapsed + 1.0f * SHIFT) / 2.0f + 0.5f,
                        std::sin(elapsed + 2.0f * SHIFT) / 2.0f + 0.5f,
                        1
                };
                renderer.draw_circle(0, 0, 300);
                renderer.end();

                renderer.begin();
                renderer.color = vec4{0, 1, 1, 1};
                renderer.transform = rotate(identity<mat4>(), elapsed, vec3{0, 0, 1});
                renderer.draw_rectangle(0, -50, 300, 100);
                renderer.end(true);

                window->flip();
            }
        });

        demo.get_looper().loop();
    }

    return 0;
}
