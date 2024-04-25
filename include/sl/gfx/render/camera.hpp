//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/render/projection.hpp"

#include "sl/gfx/primitives/basis.hpp"
#include "sl/gfx/primitives/transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace sl::gfx {

struct camera {
    transform tf;
    projection proj;

    basis basis(const basis& world) const {
        return gfx::basis{
            .x = tf.rot * world.x,
            .y = tf.rot * world.y,
            .z = tf.rot * world.z,
        };
    }

    glm::mat4 view(const gfx::basis& world) const {
        const glm::vec3 pos = tf.tr;
        const glm::vec3 forward = tf.rot * world.forward();
        const glm::vec3 up = tf.rot * world.up();
        return glm::lookAt(pos, pos + forward, up);
    }

    glm::mat4 projection(glm::ivec2 window_size) const {
        return std::visit([window_size](const auto& x) { return x.matrix(window_size); }, proj);
    }
};

} // namespace sl::gfx
