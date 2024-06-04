//
// Created by usatiynyan.
//

#include "sl/gfx/vtx/texture.hpp"
#include "sl/gfx/common/log.hpp"

#include <libassert/assert.hpp>

namespace sl::gfx {
texture::texture(texture_type type)
    : meta::finalizer<texture>{ [](texture& self) {
          // TODO(@usatiynyan): more than one texture?
          glDeleteTextures(1, &self.internal_);
          log::trace("glDeleteTextures: {}", self.internal_);
      } },
      type_{ type }, internal_{ [] {
          GLuint texture;
          glGenTextures(1, &texture);
          log::trace("glGenTextures: {}", texture);
          return texture;
      }() } {}

bound_texture texture::bind() { return bound_texture{ *this }; }

const bound_texture texture::bind() const { return bound_texture{ *this }; }

const bound_texture texture::activate(std::size_t unit) const {
    // TODO(@usatiynyan): GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
    ASSERT(unit < max_texture_units);
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit));
    return bind();
}


bound_texture::bound_texture(const texture& texture)
    :
#ifndef NDEBUG
      meta::finalizer<bound_texture>{ [](bound_texture& self) {
          glBindTexture(static_cast<GLenum>(self.type_), 0);
          log::trace("glBindTexture({}): 0", static_cast<GLenum>(self.type_));
      } },
#endif
      type_{ texture.type() } {
    glBindTexture(static_cast<GLenum>(type_), texture.internal());
    log::trace("glBindTexture({}): {}", static_cast<GLenum>(type_), texture.internal());
}

void bound_texture::set_parameter(GLenum key, GLint value) { glTexParameteri(static_cast<GLenum>(type_), key, value); }

texture_builder::texture_builder(texture_type type) : tex_{ texture{ type } }, bound_{ tl::in_place, *tex_ } {}

void texture_builder::set_wrap_s(texture_wrap wrap) {
    bound_->set_parameter(GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
}

void texture_builder::set_wrap_t(texture_wrap wrap) {
    bound_->set_parameter(GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
}

void texture_builder::set_wrap_r(texture_wrap wrap) {
    bound_->set_parameter(GL_TEXTURE_WRAP_R, static_cast<GLint>(wrap));
}

void texture_builder::set_min_filter(texture_filter filter) {
    bound_->set_parameter(GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
}

void texture_builder::set_max_filter(texture_filter filter) {
    bound_->set_parameter(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
}

texture texture_builder::submit(bool generate_mipmap) && {
    if (generate_mipmap) {
        glGenerateMipmap(static_cast<GLenum>(tex_->type()));
    }
    bound_.reset();
    return std::move(tex_).value();
}

} // namespace sl::gfx
