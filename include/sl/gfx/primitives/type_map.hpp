//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

namespace sl::gfx {
template <typename Key>
struct type_map;

template <>
struct type_map<GLbyte> {
    static constexpr GLenum value = GL_BYTE;
};

template <>
struct type_map<GLubyte> {
    static constexpr GLenum value = GL_UNSIGNED_BYTE;
};

template <>
struct type_map<GLshort> {
    static constexpr GLenum value = GL_SHORT;
};

template <>
struct type_map<GLushort> {
    static constexpr GLenum value = GL_UNSIGNED_SHORT;
};

template <>
struct type_map<GLint> {
    static constexpr GLenum value = GL_INT;
};

template <>
struct type_map<GLuint> {
    static constexpr GLenum value = GL_UNSIGNED_INT;
};

template <>
struct type_map<GLfloat> {
    static constexpr GLenum value = GL_FLOAT;
};

template <>
struct type_map<GLdouble> {
    static constexpr GLenum value = GL_DOUBLE;
};
} // namespace sl::gfx
