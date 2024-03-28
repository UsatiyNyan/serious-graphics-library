//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/primitives/transform.hpp"


namespace sl::gfx {

struct Camera {
    Transform tf;
    glm::vec3 world_up;

    glm::vec3 front() const { return glm::normalize(tf.rot * world_up); }
    glm::vec3 right() const { return glm::normalize(glm::cross(front(), world_up)); }
    glm::vec3 up() const { return glm::normalize(glm::cross(right(), front())); }
    glm::mat4 view() const { return glm::lookAt(tf.tr, tf.tr + front(), up()); }
};

} // namespace sl::gfx
