//
// Created by usatiynyan on 12/17/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

#include "sl/meta/field/alignment.hpp"
#include "sl/meta/tuple/decay.hpp"
#include "sl/meta/tuple/tie_as_tuple.hpp"

namespace sl::gfx {

struct VertexArrayAttribute {
    // TODO(@usatiynyan): retrieve in configuration glGetIntegerv(GL_MAX_VERTEX_ATTRIBS)
    static constexpr std::size_t MAX_ATTRIBS = 16;

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
