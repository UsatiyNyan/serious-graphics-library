//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/primitives/transform.hpp"


namespace sl::gfx {

struct Camera {
    Transform tf;
    glm::vec3 up;

    glm::vec3 front() const { return tf.rot * up; }
    glm::vec3 right() const { return glm::cross(front(), up); }
    glm::mat4 view() const { return glm::lookAt(tf.tr, tf.tr + front(), up); }
};

} // namespace sl::gfx
