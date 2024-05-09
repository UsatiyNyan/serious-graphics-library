//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/gl_type_map.hpp"
#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/vtx/texture.hpp"
#include "sl/gfx/vtx/vertex_array.hpp"

#include <sl/meta/tuple/for_each_meta_enumerate.hpp>

namespace sl::gfx {

template <texture_type... tex_types>
class draw {
    template <std::size_t unit>
    struct activate_texture_unit {
        template <texture_type type>
        [[nodiscard]] constexpr auto operator()(const texture<type>& tex) {
            return tex.template activate<unit>();
        }
    };

public:
    draw(bound_shader_program bound_sp, const vertex_array& va, std::tuple<const texture<tex_types>&...> texs)
        : bound_sp_{ std::move(bound_sp) }, bound_va_{ va },
          bound_texs_{ meta::for_each_meta_enumerate<activate_texture_unit>(texs) } {}

    [[nodiscard]] const auto& sp() const { return bound_sp_; }
    [[nodiscard]] const auto& va() const { return bound_va_; }
    [[nodiscard]] const auto& texs() const { return bound_texs_; }

    template <typename T, buffer_usage usage>
    void elements(const buffer<T, buffer_type::element_array, usage>& eb) {
        glDrawElements(GL_TRIANGLES, eb.data_size(), gl_type_query<T>, nullptr);
    }

private:
    bound_shader_program bound_sp_;
    bound_vertex_array bound_va_;
    std::tuple<bound_texture<tex_types>...> bound_texs_;
};

} // namespace sl::gfx
