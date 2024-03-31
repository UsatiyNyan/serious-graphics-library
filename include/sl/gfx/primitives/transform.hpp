//
// Created by usatiynyan.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sl::gfx {

struct Transform {
    glm::vec3 tr;
    glm::quat rot;

    void translate(const glm::vec3& a_tr) { tr += a_tr; }
    void rotate(const glm::quat& a_rot) { rot = glm::normalize(a_rot * rot); }
    void normalize() {
        if (glm::length2(tr) > 0.0f) {
            tr = glm::normalize(tr);
        }
        rot = glm::normalize(rot);
    }
};

inline Transform translate(Transform tf, const glm::vec3& tr) {
    tf.translate(tr);
    return tf;
}
inline Transform rotate(Transform tf, const glm::quat& rot) {
    tf.rotate(rot);
    return tf;
}
inline Transform normalize(Transform tf) {
    tf.normalize();
    return tf;
}

} // namespace sl::gfx
