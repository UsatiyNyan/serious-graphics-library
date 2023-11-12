//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/common/vendors.hpp"

namespace sl::gfx {

// TODO: std::array or std::span view
struct Color4F {
    GLfloat red;
    GLfloat green;
    GLfloat blue;
    GLfloat alpha;
};

} // namespace sl::gfx
