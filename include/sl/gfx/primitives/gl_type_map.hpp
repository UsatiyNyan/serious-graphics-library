//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

#include <sl/meta/match/overloaded.hpp>

namespace sl::gfx {

constexpr meta::overloaded gl_type_map{
    [](GLbyte) { return GL_BYTE; }, //
    [](GLubyte) { return GL_UNSIGNED_BYTE; }, //
    [](GLshort) { return GL_SHORT; }, //
    [](GLushort) { return GL_UNSIGNED_SHORT; }, //
    [](GLint) { return GL_INT; }, //
    [](GLuint) { return GL_UNSIGNED_INT; }, //
    [](GLfloat) { return GL_FLOAT; }, //
    [](GLdouble) { return GL_DOUBLE; }, //
};

template <typename GLType>
constexpr GLenum gl_type_query = gl_type_map(GLType{});

} // namespace sl::gfx
