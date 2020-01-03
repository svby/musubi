/// @file
/// @author stuhlmeier
/// @date 7 December 2019

#include <musubi/camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    constexpr glm::vec3 UP{0.0f, 1.0f, 0.0f};
}

namespace musubi {
    camera::camera() noexcept = default;

    camera &camera::set_viewport_ortho(const float width, const float height) noexcept {
        storedWidth = width;
        storedHeight = height;
        projection = glm::ortho(0.0f, width, 0.0f, height);
        set_position(storedX, storedY);

        return *this;
    }

    camera &camera::set_position(float x, float y) noexcept {
        storedX = x;
        storedY = y;
        view = glm::lookAt(
                glm::vec3{x - storedWidth / 2, y - storedHeight / 2, 1.0f},
                glm::vec3{x - storedWidth / 2, y - storedHeight / 2, 0.0f},
                UP
        );

        return *this;
    }

    float camera::get_x() const noexcept { return storedX; }

    float camera::get_y() const noexcept { return storedY; }

    float camera::get_width() const noexcept { return storedWidth; }

    float camera::get_height() const noexcept { return storedHeight; }
}
