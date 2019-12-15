#include <musubi/application.h>
#include <musubi/pixmap.h>
#include <musubi/gl/shaders.h>
#include <musubi/gl/shapes.h>
#include <musubi/gl/textures.h>
#include <musubi/sdl/sdl_init.h>
#include <musubi/sdl/sdl_window.h>

#include <epoxy/gl.h>
#include <glm/ext/matrix_transform.hpp>

#include <chrono>
#include <cmath>
#include <thread>

using namespace musubi;
using namespace std::chrono;
using glm::mat4, glm::vec3, glm::vec4;
using glm::identity, glm::rotate, glm::translate;

constexpr float hsv_to_rgb_k(uint32 n, float hue) {
    constexpr float PI_3{pi<float> / 3.0f};
    return std::fmod(n + (hue / PI_3), 6.0f);
}

constexpr float hsv_to_rgb_f(uint32 n, float hue, float saturation, float value) {
    const auto k = hsv_to_rgb_k(n, hue);
    return value - value * saturation * std::max(std::min(k, std::min(4.0f - k, 1.0f)), 0.0f);
}

constexpr uint32 hsv_to_rgba(float hue, float saturation, float value) {
    return rgba8(
            255 * hsv_to_rgb_f(5, hue, saturation, value),
            255 * hsv_to_rgb_f(3, hue, saturation, value),
            255 * hsv_to_rgb_f(1, hue, saturation, value)
    );
}

int main() {
    {
        buffer_pixmap<pixmap_format::rgba8> pixmap(1280, 720);

        const auto midX = pixmap.get_width() / 2;
        const auto midY = pixmap.get_height() / 2;
        const auto minDim = std::min(pixmap.get_width(), pixmap.get_height());
        const auto radius = std::max(minDim - 100.0f, 100.0f);

        for (uint32 x = 0; x < pixmap.get_width(); ++x) {
            for (uint32 y = 0; y < pixmap.get_height(); ++y) {
                const float dX = static_cast<float>(x) - midX, dY = static_cast<float>(y) - midY;

                const auto hue = std::atan2(-dY, dX);
                const auto saturation =
                        std::clamp(std::sqrt(dX * dX + dY * dY) / radius, 0.0f, 1.0f);

                pixmap.set_pixel(x, y, hsv_to_rgba(hue, saturation, 1));
            }
        }

        sdl::scoped_init disposer;

        application demo;

        const auto windowPtr = demo.create_window<sdl::sdl_window>(window::start_info{
                .width = 1280,
                .height = 720,
                .title = "musubi_demo",
                .window_mode = window_mode::windowed
        });

        const auto texture{std::make_shared<musubi::gl::texture>(pixmap, GL_RGBA8)};

        camera camera;
        camera
                .set_viewport_ortho(1280, 720)
                .set_position(0, 0);

        gl::gl_shape_renderer shapes;
        shapes.init();

        gl::gl_texture_renderer textures;
        textures.init();

        shapes.camera = textures.camera = camera;

        const auto start = high_resolution_clock::now();
        demo.get_looper().add_action([&](auto &token) {
            const auto elapsed = duration_cast<duration<float>>(high_resolution_clock::now() - start).count();
            const auto SHIFT = 2.0f / 3.0f * pi<float>;

            if (auto window = windowPtr.lock()) {
                window->make_current();

                glClearColor(0.5f, 0.5f, 0.5f, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                textures.begin_batch(texture);
                textures.batch_draw_texture(-640, -360, 1280, 720);
                textures.end_batch(false);

                const auto x = 500 * std::sin(elapsed);
                const auto y = 220 * std::cos(2 * pi<float> * elapsed);
                constexpr auto hl = 50.0f;
                const auto squareTransform = rotate(translate(identity<mat4>(), {x, y, 0}), 4 * elapsed,
                                                    {0, 0, 1});

                textures.begin_batch(texture);
                textures.transform = squareTransform;
                textures.batch_draw_texture(-hl, -hl, hl * 2, hl * 2);
                textures.end_batch(true);

                shapes.begin_batch();
                shapes.color = {0, 0, 0, 1};
                shapes.transform = squareTransform;
                shapes.batch_draw_line(-hl, -hl, -hl, hl);
                shapes.batch_draw_line(-hl, hl, hl, hl);
                shapes.batch_draw_line(hl, hl, hl, -hl);
                shapes.batch_draw_line(hl, -hl, -hl, -hl);
                shapes.end_batch(true);

                shapes.begin_batch();
                shapes.color = {
                        std::sin(elapsed + 0.0f * SHIFT) / 2.0f + 0.5f,
                        std::sin(elapsed + 1.0f * SHIFT) / 2.0f + 0.5f,
                        std::sin(elapsed + 2.0f * SHIFT) / 2.0f + 0.5f,
                        1
                };
                shapes.batch_draw_circle(0, 0, 300);
                shapes.end_batch(false);

                shapes.begin_batch();
                shapes.color = {0, 1, 1, 1};
                shapes.transform = rotate(identity<mat4>(), elapsed, {0, 0, 1});
                shapes.batch_draw_rectangle(0, -50, 300, 100);
                shapes.end_batch(true);

                window->flip();
            }
        });

        demo.get_looper().loop();
    }

    return 0;
}
