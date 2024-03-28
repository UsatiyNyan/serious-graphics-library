//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/primitives/size.hpp"

#include <glm/gtc/matrix_transform.hpp>


namespace sl::gfx {

enum class ProjectionType { Perspective, Orthographic };

template <ProjectionType type>
struct Projection {
    float fov; // radians
    float near;
    float far;

    glm::mat4 matrix(Size2I window_size) const {
        if constexpr (type == ProjectionType::Perspective) {
            return glm::perspective(fov, window_size.aspect_ratio(), near, far);
        } else {
            return glm::ortho(
                0.0f, static_cast<float>(window_size.width), static_cast<float>(window_size.height), 0.0f
            );
        }
    }
};

} // namespace sl::gfx
