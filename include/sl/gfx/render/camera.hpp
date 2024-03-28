//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/primitives/transform.hpp"


namespace sl::gfx {

struct Camera {
    Transform tf;
    glm::vec3 world_up;

    struct State {
        glm::vec3 pos;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;

        glm::mat4 view() const { return glm::lookAt(pos, pos + front, up); }
    };

    State state() const {
        glm::vec3 front = glm::normalize(tf.rot * world_up);
        glm::vec3 right = glm::normalize(glm::cross(front, world_up));
        glm::vec3 up = glm::normalize(glm::cross(right, front));
        return State{
            .pos = tf.tr,
            .front = front,
            .right = right,
            .up = up,
        };
    }
};

} // namespace sl::gfx
