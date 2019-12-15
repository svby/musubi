//
// Created by stuhlmeier on 12/13/19.
//

#include <musubi/renderer.h>

namespace musubi {
    renderer::renderer() = default;

    renderer::~renderer() = default;

    renderer::renderer(const renderer &other) = default;

    renderer &renderer::operator=(const renderer &other) = default;

    renderer::renderer(renderer &&other) noexcept = default;

    renderer &renderer::operator=(renderer &&other) noexcept = default;
}
