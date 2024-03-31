//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/primitives/size.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <variant>


namespace sl::gfx {

struct PerspectiveProjection {
    float fov; // radians
    float near;
    float far;
    glm::mat4 matrix(Size2I window_size) const { return glm::perspective(fov, window_size.aspect_ratio(), near, far); }
};

struct OrthographicProjection {
    glm::mat4 matrix(Size2I window_size) const {
        return glm::ortho(0.0f, static_cast<float>(window_size.width), static_cast<float>(window_size.height), 0.0f);
    }
};

using Projection = std::variant<PerspectiveProjection, OrthographicProjection>;

} // namespace sl::gfx
