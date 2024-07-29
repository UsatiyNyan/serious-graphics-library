//
// Created by usatiynyan on 12/17/23.
//

#pragma once

#include "sl/gfx/detail/vendors.hpp"

#include <sl/meta/field/alignment.hpp>

namespace sl::gfx {

// TODO(@usatiynyan): retrieve in configuration glGetIntegerv(GL_MAX_VERTEX_ATTRIBS)
constexpr std::size_t max_vertex_array_attributes = 16;

// vertex array attribute
struct vertex_array_attribute {
    GLuint index;
    GLint components_count;
    GLenum type;
    GLsizei offset;
    GLsizei stride;
    bool normalized;
};

template <GLint component_count>
constexpr bool is_allowed_component_count = (1 <= component_count && component_count <= 4);

template <GLint component_count_, typename T, typename AlignAsT = T, bool normalized_ = false>
    requires is_allowed_component_count<component_count_>
struct va_attrib_field : meta::aligned_field<T[component_count_], AlignAsT> {};

} // namespace sl::gfx
