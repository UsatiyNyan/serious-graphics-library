//
// Created by usatiynyan on 12/2/23.
//

#pragma once

#include "sl/gfx/detail/gl_type_map.hpp"
#include "sl/gfx/detail/vendors.hpp"

#include <sl/meta/func/lazy_eval.hpp>
#include <sl/meta/lifetime/finalizer.hpp>
#include <sl/meta/traits/unique.hpp>

#include <boost/container/small_vector.hpp>
#include <range/v3/view/enumerate.hpp>
#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {

// TODO(@usatiynyan): GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
constexpr std::size_t max_texture_units = 16;

enum class texture_type : GLenum {
    texture_1d = GL_TEXTURE_1D,
    texture_2d = GL_TEXTURE_2D,
    texture_3d = GL_TEXTURE_3D,
    texture_1d_array = GL_TEXTURE_1D_ARRAY,
    texture_2d_array = GL_TEXTURE_2D_ARRAY,
    texture_rectangle = GL_TEXTURE_RECTANGLE,
    texture_cube_map = GL_TEXTURE_CUBE_MAP,
    texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
    texture_buffer = GL_TEXTURE_BUFFER,
    texture_2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
    texture_2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
};

enum class texture_wrap : GLint {
    repeat = GL_REPEAT,
    mirrored_repeat = GL_MIRRORED_REPEAT,
    clamp_to_edge = GL_CLAMP_TO_EDGE,
    clamp_to_border = GL_CLAMP_TO_BORDER,
    mirror_clamp_to_edge = GL_MIRROR_CLAMP_TO_EDGE,
};

enum class texture_filter : GLint {
    nearest = GL_NEAREST,
    linear = GL_LINEAR,
    nearest_mipmap_nearest = GL_NEAREST_MIPMAP_NEAREST,
    linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST,
    nearest_mipmap_linear = GL_NEAREST_MIPMAP_LINEAR,
    linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR,
};

struct texture_format {
    // TODO(types): enum types and size mapping
    explicit texture_format(GLenum internal_and_input)
        : internal{ static_cast<GLint>(internal_and_input) }, input{ internal_and_input } {}

    texture_format(GLint an_internal, GLenum an_input) : internal{ an_internal }, input{ an_input } {}

public:
    GLint internal;
    GLenum input;
};

class texture_builder;

struct bound_texture;

class texture : public meta::finalizer<texture> {
    friend class texture_builder;

    explicit texture(texture_type type);

public:
    [[nodiscard]] bound_texture bind();
    [[nodiscard]] bound_texture bind() const;
    [[nodiscard]] bound_texture activate(std::size_t unit) const;

    [[nodiscard]] texture_type type() const { return type_; }
    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    texture_type type_;
    GLuint internal_;
};

struct bound_texture : public meta::finalizer<bound_texture> {
    explicit bound_texture(const texture& texture);

    void set_parameter(GLenum key, GLint value);

    [[nodiscard]] const texture* operator->() const { return &texture_; }

private:
    const texture& texture_;
};

class texture_builder : public meta::unique {
public:
    explicit texture_builder(texture_type type);

    void set_wrap_s(texture_wrap wrap);
    void set_wrap_t(texture_wrap wrap);
    void set_wrap_r(texture_wrap wrap);

    void set_min_filter(texture_filter filter);
    void set_max_filter(texture_filter filter);

    template <std::size_t dimensions_extent, typename T>
    void set_image(
        std::span<const GLsizei, dimensions_extent> dimensions,
        texture_format format,
        const T* data,
        GLint lod = 0
    );

    texture submit(bool generate_mipmap = true) &&;

private:
    tl::optional<texture> tex_;
    tl::optional<bound_texture> bound_;
};

template <std::size_t dimensions_extent, typename T>
void texture_builder::set_image(
    std::span<const GLsizei, dimensions_extent> dimensions,
    texture_format format,
    const T* data,
    GLint lod
) {
    const GLenum tex_type = static_cast<GLenum>(tex_->type());
    if constexpr (dimensions_extent == 1ul) {
        const GLsizei x = dimensions[0];
        glTexImage1D(tex_type, lod, format.internal, x, 0, format.input, gl_type_query<T>, data);
    } else if constexpr (dimensions_extent == 2ul) {
        const GLsizei x = dimensions[0];
        const GLsizei y = dimensions[1];
        glTexImage2D(tex_type, lod, format.internal, x, y, 0, format.input, gl_type_query<T>, data);
    } else if constexpr (dimensions_extent == 3ul) {
        const GLsizei x = dimensions[0];
        const GLsizei y = dimensions[1];
        const GLsizei z = dimensions[2];
        glTexImage3D(tex_type, lod, format.internal, x, y, z, 0, format.input, gl_type_query<T>, data);
    } else {
        static_assert(dimensions_extent <= 3ul, "only up to 3 dimensions");
    }
}

using bound_textures = boost::container::small_vector<bound_texture, max_texture_units>;

template <typename texture_range>
bound_textures activate_textures(texture_range&& textures) {
    bound_textures bound_textures;
    for (const auto& [unit, texture] : ranges::views::enumerate(std::forward<texture_range>(textures))) {
        bound_textures.emplace_back(meta::lazy_eval{ [&u = unit, &t = texture] { return t.activate(u); } });
    }
    return bound_textures;
}

} // namespace sl::gfx
