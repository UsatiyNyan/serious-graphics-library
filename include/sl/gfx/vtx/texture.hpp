//
// Created by usatiynyan on 12/2/23.
//

#pragma once

#include "sl/gfx/common/log.hpp"
#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/gl_type_map.hpp"

#include <sl/meta/lifetime/finalizer.hpp>
#include <sl/meta/lifetime/unique.hpp>

#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {

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

template <texture_type type>
class bound_texture;

template <texture_type type>
class texture : public meta::finalizer<texture<type>> {
    texture()
        : meta::finalizer<texture>{ [](texture& self) {
              // TODO(@usatiynyan): more than one texture?
              glDeleteTextures(1, &self.internal_);
              LOG_DEBUG("glDeleteTextures: {}", self.internal_);
          } },
          internal_{ [] {
              GLuint texture;
              glGenTextures(1, &texture);
              LOG_DEBUG("glGenTextures: {}", texture);
              return texture;
          }() } {}

public:
    [[nodiscard]] bound_texture<type> bind();
    [[nodiscard]] const bound_texture<type> bind() const;
    template <std::size_t unit>
    [[nodiscard]] const bound_texture<type> activate() const;

    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    GLuint internal_;
};

template <texture_type type>
class bound_texture
    : public
#ifdef NDEBUG
      meta::unique
#else
      meta::finalizer<bound_texture<type>>
#endif
{
public:
    explicit bound_texture(const texture<type>& texture)
#ifndef NDEBUG
        : meta::finalizer<bound_texture>{ [](bound_texture&) {
              glBindTexture(static_cast<GLenum>(type), 0);
              LOG_DEBUG("glBindTexture({}): 0", static_cast<GLenum>(type));
          } }
#endif
    {
        glBindTexture(static_cast<GLenum>(type), texture.internal());
        LOG_DEBUG("glBindTexture({}): {}", static_cast<GLenum>(type), texture.internal());
    }

    void set_parameter(GLenum key, GLint value) { glTexParameteri(static_cast<GLenum>(type), key, value); }
};

template <texture_type type>
bound_texture<type> texture<type>::bind() {
    return bound_texture{ *this };
}

template <texture_type type>
const bound_texture<type> texture<type>::bind() const {
    return bound_texture{ *this };
}

template <texture_type type>
template <std::size_t unit>
const bound_texture<type> texture<type>::activate() const {
    // TODO(@usatiynyan): static_assert(index < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    static_assert(unit < 16, "texture_units can be < 16, if this is incorrect check TODO above");
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit));
    return bind();
}

template <texture_type type>
class texture_builder : public meta::unique {
public:
    void set_wrap_s(texture_wrap wrap) { bound_->set_parameter(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap)); }
    void set_wrap_t(texture_wrap wrap) { bound_->set_parameter(GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap)); }
    void set_wrap_r(texture_wrap wrap) { bound_->set_parameter(GL_TEXTURE_WRAP_R, static_cast<GLint>(wrap)); }

    void set_min_filter(texture_filter filter) {
        bound_->set_parameter(GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    }
    void set_max_filter(texture_filter filter) {
        bound_->set_parameter(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    }

    template <std::size_t dimensions_extent, typename T>
    void set_image(
        std::span<const GLsizei, dimensions_extent> dimensions,
        texture_format format,
        const T* data,
        GLint lod = 0
    ) {
        if constexpr (dimensions_extent == 1ul) {
            glTexImage1D(
                static_cast<GLenum>(type), lod, format.internal, dimensions[0], 0, format.input, gl_type_query<T>, data
            );
        } else if constexpr (dimensions_extent == 2ul) {
            glTexImage2D(
                static_cast<GLenum>(type),
                lod,
                format.internal,
                dimensions[0],
                dimensions[1],
                0,
                format.input,
                gl_type_query<T>,
                data
            );
        } else if constexpr (dimensions_extent == 3ul) {
            glTexImage3D(
                static_cast<GLenum>(type),
                lod,
                format.internal,
                dimensions[0],
                dimensions[1],
                dimensions[2],
                0,
                format.input,
                gl_type_query<T>,
                data
            );
        } else {
            static_assert(dimensions_extent <= 3ul, "only up to 3 dimensions");
        }
    }

    texture<type> submit(bool generate_mipmap = true) && {
        if (generate_mipmap) {
            glGenerateMipmap(static_cast<GLenum>(type));
        }
        bound_.reset();
        return std::move(tex_).value();
    }

private:
    tl::optional<texture<type>> tex_{ texture<type>{} };
    tl::optional<bound_texture<type>> bound_{ tl::in_place, *tex_ };
};

} // namespace sl::gfx
