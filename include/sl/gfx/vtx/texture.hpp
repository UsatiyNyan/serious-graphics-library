//
// Created by usatiynyan on 12/2/23.
//

#pragma once

#include "sl/meta/lifetime/finalizer.hpp"
#include "sl/gfx/common/log.hpp"
#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/type_map.hpp"

#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {

enum class TextureType : GLenum {
    TEXTURE_1D = GL_TEXTURE_1D,
    TEXTURE_2D = GL_TEXTURE_2D,
    TEXTURE_3D = GL_TEXTURE_3D,
    TEXTURE_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
    TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
    TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE,
    TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
    TEXTURE_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
    TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
    TEXTURE_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
    TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
};

enum class TextureWrap : GLint {
    REPEAT = GL_REPEAT,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
    MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE,
};

enum class TextureFilter : GLint {
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR,
    NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

struct TextureFormat {
    // TODO(types): enum types and size mapping
    explicit TextureFormat(GLenum internal_and_input)
        : internal{ static_cast<GLint>(internal_and_input) }, input{ internal_and_input } {}

    TextureFormat(GLint an_internal, GLenum an_input) : internal{ an_internal }, input{ an_input } {}

public:
    GLint internal;
    GLenum input;
};

template <TextureType type_>
class Texture : public finalizer<Texture<type_>> {
    template <TextureType>
    friend class TextureBuilder;

    Texture()
        : finalizer<Texture>{ [](Texture& self) {
              // TODO(@usatiynyan): more than one texture?
              glDeleteTextures(1, &self.object_);
              LOG_DEBUG("glDeleteTextures: {}", self.object_);
          } },
          object_{ [] {
              GLuint texture;
              glGenTextures(1, &texture);
              LOG_DEBUG("glGenTextures: {}", texture);
              return texture;
          }() } {}

public:
    class ConstBind : public finalizer<ConstBind> {
    public:
        explicit ConstBind(const Texture& tex) : finalizer<ConstBind> {
            [](ConstBind&) {
                // TODO(@usatiynyan): can be omitted in "optimized" build
                glBindTexture(static_cast<GLenum>(type_), 0);
                LOG_DEBUG("glBindTexture({}): 0", static_cast<GLenum>(type_));
            }
        }
        {
            glBindTexture(static_cast<GLenum>(type_), *tex);
            LOG_DEBUG("glBindTexture({}): {}", static_cast<GLenum>(type_), *tex);
        }
    };

    class Bind : private ConstBind {
    public:
        explicit Bind(Texture& tex) : ConstBind{ tex } {}

        void set_parameter(GLenum key, GLint value) { glTexParameteri(static_cast<GLenum>(type_), key, value); }
    };

public:
    [[nodiscard]] GLuint operator*() const { return object_; }
    [[nodiscard]] auto bind() const { return ConstBind{ *this }; }
    [[nodiscard]] auto bind() { return Bind{ *this }; }
    template <std::size_t unit_>
    [[nodiscard]] auto activate() const {
        // TODO(@usatiynyan): static_assert(index < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
        static_assert(unit_ < 16, "texture_units can be < 16, if this is incorrect check TODO above");
        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit_));
        return bind();
    }

private:
    GLuint object_;
};

template <TextureType type_>
class TextureBuilder {
public:
    void set_wrap_s(TextureWrap wrap) { bind_->set_parameter(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap)); }
    void set_wrap_t(TextureWrap wrap) { bind_->set_parameter(GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap)); }
    void set_wrap_r(TextureWrap wrap) { bind_->set_parameter(GL_TEXTURE_WRAP_R, static_cast<GLint>(wrap)); }

    void set_min_filter(TextureFilter filter) {
        bind_->set_parameter(GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    }
    void set_max_filter(TextureFilter filter) {
        bind_->set_parameter(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    }

    template <std::size_t dimensions_count, typename DataType>
    void set_image(
        const std::array<GLsizei, dimensions_count>& dimensions,
        TextureFormat format,
        const DataType* data,
        GLint lod = 0
    ) {
        if constexpr (dimensions_count == 1ul) {
            glTexImage1D(
                static_cast<GLenum>(type_),
                lod,
                format.internal,
                dimensions[0],
                0,
                format.input,
                type_map<DataType>::value,
                data
            );
        } else if constexpr (dimensions_count == 2ul) {
            glTexImage2D(
                static_cast<GLenum>(type_),
                lod,
                format.internal,
                dimensions[0],
                dimensions[1],
                0,
                format.input,
                type_map<DataType>::value,
                data
            );
        } else if constexpr (dimensions_count == 3ul) {
            glTexImage3D(
                static_cast<GLenum>(type_),
                lod,
                format.internal,
                dimensions[0],
                dimensions[1],
                dimensions[2],
                0,
                format.input,
                type_map<DataType>::value,
                data
            );
        } else {
            static_assert(dimensions_count <= 3ul, "only up to 3 dimensions");
        }
    }

    Texture<type_> submit(bool generate_mipmap = true) && {
        if (generate_mipmap) {
            glGenerateMipmap(static_cast<GLenum>(type_));
        }
        bind_.reset();
        return std::move(tex_).value();
    }

private:
    tl::optional<Texture<type_>> tex_{ Texture<type_>{} };
    tl::optional<typename Texture<type_>::Bind> bind_{ tl::in_place, *tex_ };
};

} // namespace sl::gfx
