//
// Created by usatiynyan.
//

#pragma once

#include "sl_example/basis.hpp"
#include "sl_example/projection.hpp"
#include "sl_example/transform.hpp"

namespace sl::gfx {

// TODO: remove, this struct is redundant
struct camera {
    transform tf;
    projection proj;

    glm::mat4 view(const gfx::basis& world) const { return world.view(tf); }
    glm::mat4 projection(glm::ivec2 window_size) const { return proj.calculate(window_size); }
};

} // namespace sl::gfx
