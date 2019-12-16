//
// Created by stuhlmeier on 12/13/19.
//

#ifndef MUSUBI_RENDERER_H
#define MUSUBI_RENDERER_H

#include "musubi/camera.h"

#include <glm/mat4x4.hpp>

namespace musubi {
    class renderer {
    protected:
        renderer();

    public:
        ::musubi::camera camera{};
        glm::mat4 transform{1.0f};

        virtual ~renderer();

        renderer(const renderer &other);

        renderer &operator=(const renderer &other);

        renderer(renderer &&other) noexcept;

        renderer &operator=(renderer &&other) noexcept;

        virtual void init() = 0;
    };
}

#endif //MUSUBI_RENDERER_H
