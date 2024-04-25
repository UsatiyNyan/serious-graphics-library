//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/ctx/window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <variant>

namespace sl::gfx {

struct perspective_projection {
    float fov; // radians
    float near;
    float far;
    glm::mat4 matrix(glm::ivec2 window_size) const {
        return glm::perspective(fov, aspect_ratio<float>(window_size), near, far);
    }
};

struct orthographic_projection {
    glm::mat4 matrix(glm::ivec2 window_size) const {
        return glm::ortho(0.0f, static_cast<float>(window_size.x), static_cast<float>(window_size.x), 0.0f);
    }
};

using projection = std::variant<perspective_projection, orthographic_projection>;

} // namespace sl::gfx
