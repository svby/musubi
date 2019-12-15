//
// Created by stuhlmeier on 12/7/19.
//

#ifndef MUSUBI_CAMERA_H
#define MUSUBI_CAMERA_H

#include <glm/mat4x4.hpp>

namespace musubi {
    struct camera final {
        glm::mat4 projection{}, view{};

        camera() noexcept;

        camera &set_viewport_ortho(float width, float height) noexcept;

        camera &set_position(float x, float y) noexcept;

        [[nodiscard]] float get_x() const noexcept;

        [[nodiscard]] float get_y() const noexcept;

        [[nodiscard]] float get_width() const noexcept;

        [[nodiscard]] float get_height() const noexcept;

    private:
        float storedX{0}, storedY{0}, storedWidth{16}, storedHeight{9};
    };
}

#endif //MUSUBI_CAMERA_H
