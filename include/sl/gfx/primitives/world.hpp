//
// Created by usatiynyan.
//

#pragma once

#include <glm/glm.hpp>


namespace sl::gfx {

struct World {
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 forward{ 0.0f, 0.0f, 1.0f };
    glm::vec3 right = glm::cross(up, forward);
};

} // namespace sl::gfx
