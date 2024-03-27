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
    void rotate(const glm::quat& a_rot) { rot *= a_rot; }
};

inline Transform translate(const Transform& tf, const glm::vec3& tr) { return { tf.tr + tr, tf.rot }; }
inline Transform rotate(const Transform& tf, const glm::quat& rot) { return { tf.tr, tf.rot * rot }; }

} // namespace sl::gfx
