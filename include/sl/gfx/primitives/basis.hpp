//
// Created by usatiynyan.
//

#pragma once

#include <glm/glm.hpp>

namespace sl::gfx {

struct basis {
    glm::vec3 x{ 1.0f, 0.0f, 0.0f };
    glm::vec3 y{ 0.0f, 1.0f, 0.0f };
    glm::vec3 z{ 0.0f, 0.0f, 1.0f };

    glm::vec3 forward() const { return z; }
    glm::vec3 right() const { return -x; }
    glm::vec3 up() const { return y; }
};

} // namespace sl::gfx
