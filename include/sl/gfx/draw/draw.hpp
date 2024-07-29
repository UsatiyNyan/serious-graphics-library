//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/detail/gl_type_map.hpp"
#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/vtx/texture.hpp"
#include "sl/gfx/vtx/vertex_array.hpp"

namespace sl::gfx {

class draw {
public:
    // requirements for drawing
    draw(const bound_shader_program&, const bound_vertex_array&) {}
    // requirements for drawing
    draw(const bound_shader_program&, const bound_vertex_array&, const bound_textures&) {}

    template <typename T, buffer_usage usage>
    void elements(const buffer<T, buffer_type::element_array, usage>& eb) {
        glDrawElements(GL_TRIANGLES, eb.data_size(), gl_type_query<T>, nullptr);
    }
};

} // namespace sl::gfx
