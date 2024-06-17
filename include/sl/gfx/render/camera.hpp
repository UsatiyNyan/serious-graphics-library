//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/render/projection.hpp"

#include "sl/gfx/primitives/basis.hpp"
#include "sl/gfx/primitives/transform.hpp"

namespace sl::gfx {

// TODO: remove, this struct is redundant
struct camera {
    transform tf;
    projection proj;

    glm::mat4 view(const gfx::basis& world) const { return world.view(tf); }
    glm::mat4 projection(glm::ivec2 window_size) const { return proj.calculate(window_size); }
};

} // namespace sl::gfx
