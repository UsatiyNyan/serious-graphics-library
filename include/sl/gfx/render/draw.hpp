//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/gl_type_map.hpp"
#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/vtx/texture.hpp"
#include "sl/gfx/vtx/vertex_array.hpp"

#include <boost/container/small_vector.hpp>
#include <range/v3/view/enumerate.hpp>
#include <sl/meta/lifetime/lazy_eval.hpp>

namespace sl::gfx {

class draw {
    using bound_textures_container = boost::container::small_vector<bound_texture, max_texture_units>;

public:
    template <std::size_t textures_extent = 0>
    draw(
        const bound_shader_program&,
        const bound_vertex_array&,
        std::span<const texture, textures_extent> textures = {}
    )
        : bound_textures_{ [textures] {
              bound_textures_container bound_textures;
              for (const auto& [unit, texture] : ranges::views::enumerate(textures)) {
                  bound_textures.emplace_back(meta::lazy_eval{ [&u = unit, &t = texture] { return t.activate(u); } });
              }
              return bound_textures;
          }() } {}

    [[nodiscard]] const auto& texs() const { return bound_textures_; }

    template <typename T, buffer_usage usage>
    void elements(const buffer<T, buffer_type::element_array, usage>& eb) {
        glDrawElements(GL_TRIANGLES, eb.data_size(), gl_type_query<T>, nullptr);
    }

private:
    bound_textures_container bound_textures_;
};

} // namespace sl::gfx
