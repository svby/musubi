#include <musubi/application.h>
#include <musubi/asset_registry.h>
#include <musubi/pixmap.h>
#include <musubi/screen.h>
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

struct empty_test_screen final : basic_screen {
    void on_update(float dt) override {
        glClearColor(0.5, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

struct recur_test_screen final : basic_screen {
    void on_update(float dt) override {
        get_current_window()->set_screen(std::make_unique<recur_test_screen>());
    }
};

struct clear_test_screen final : basic_screen {
    using clock_type = steady_clock;
    using delta_type = duration<float>;

    time_point<clock_type> startTime{};

    void on_update(float dt) override {
        const auto elapsed = duration_cast<delta_type>(clock_type::now() - startTime).count();
        const auto phaseShift = 2.0f / 3.0f * pi<float>;

        const auto r = std::sin(elapsed + 0.0f * phaseShift) / 2.0f + 0.5f;
        const auto g = std::sin(elapsed + 1.0f * phaseShift) / 2.0f + 0.5f;
        const auto b = std::sin(elapsed + 2.0f * phaseShift) / 2.0f + 0.5f;

        glClearColor(r, g, b, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

struct renderer_test_screen final : basic_screen {
    using clock_type = steady_clock;
    using delta_type = duration<float>;

    time_point<clock_type> startTime{};

    std::shared_ptr<gl::texture> texture{};

    gl::gl_shape_renderer shapes{};
    gl::gl_texture_renderer textures{};

    void on_attached(window *window) override {
        basic_screen::on_attached(window);

        {
            buffer_pixmap<pixmap_format::rgba8> pixmap(1280, 720);
            const auto radius = std::max(std::min(pixmap.get_width(), pixmap.get_height()) - 100.0f, 100.0f);
            for (uint32 x = 0; x < pixmap.get_width(); ++x) {
                for (uint32 y = 0; y < pixmap.get_height(); ++y) {
                    const float dX = static_cast<float>(x) - pixmap.get_width() / 2.0f,
                            dY = static_cast<float>(y) - pixmap.get_height() / 2.0f;

                    const auto hue = std::atan2(-dY, dX);
                    const auto saturation =
                            std::clamp(std::sqrt(dX * dX + dY * dY) / radius, 0.0f, 1.0f);

                    pixmap.set_pixel(x, y, hsv_to_rgba(hue, saturation, 1));
                }
            }

            texture = std::make_shared<gl::texture>(pixmap, true, GL_RGBA8);
        }

        camera camera;
        camera
                .set_viewport_ortho(1280, 720)
                .set_position(0, 0);

        shapes.init();
        textures.init();
        shapes.camera = textures.camera = camera;
    }

    void on_update(float dt) override {
        const auto elapsed = duration_cast<delta_type>(clock_type::now() - startTime).count();

        glClearColor(0.5, 0.5, 0.5, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        textures.begin_batch(texture);
        musubi::gl::texture_region region(texture, (std::sin(elapsed) / 4 + 0.25f),
                                          (std::sin(elapsed) / 4 + 0.25f), 1, 1);
        textures.batch_draw_region(region, -640, -360, 1280, 720);
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

        const auto phaseShift = 2.0f / 3.0f * pi<float>;
        shapes.begin_batch();
        shapes.color = {
                std::sin(elapsed + 0.0f * phaseShift) / 2.0f + 0.5f,
                std::sin(elapsed + 1.0f * phaseShift) / 2.0f + 0.5f,
                std::sin(elapsed + 2.0f * phaseShift) / 2.0f + 0.5f,
                1
        };
        shapes.batch_draw_circle(0, 0, 300);
        shapes.end_batch(false);

        shapes.begin_batch();
        shapes.color = {0, 1, 1, 1};
        shapes.transform = rotate(identity<mat4>(), elapsed, {0, 0, 1});
        shapes.batch_draw_rectangle(0, -50, 300, 100);
        shapes.end_batch(true);
    }
};

struct asset_test_screen : public basic_screen {
    std::unique_ptr<asset_registry> assets;

    void on_attached(window *window) override {
        assets = asset_registry::from_paths({"."});
        const auto pack = assets->load_pack("test");
        const auto &testFile = pack->get_item("test.txt")->get().get_buffer()->get();
        const auto data = reinterpret_cast<const char *>(testFile.data());

        std::cout << "Loaded from test.txt: "
                  << std::string(data, data + testFile.size()) << '\n';
    }
};

int main() {
    sdl::scoped_init disposer;

    application demo;

    const auto windowPtr = demo.create_window<sdl::sdl_window>(window::start_info{
            .title = "musubi_demo",
            .width = 1280,
            .height = 720,
            .mode = window_mode::windowed
    }, std::make_unique<asset_test_screen>());

    demo.get_looper().loop();
}
