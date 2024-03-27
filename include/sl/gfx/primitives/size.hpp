//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

#include <concepts>

namespace sl::gfx {

// TODO: std::array or std::span view
struct Size2I {
    GLsizei width;
    GLsizei height;

    template <typename T = float>
        requires std::is_arithmetic_v<T>
    T aspect_ratio() const {
        return static_cast<T>(width) / static_cast<T>(height);
    }
};

} // namespace sl::gfx
