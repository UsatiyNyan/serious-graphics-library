//
// Created by usatiynyan.
//

#pragma once

#include "sl/gfx/vtx/texture.hpp"

#include "sl/gfx/detail/log.hpp"
#include "sl/gfx/detail/vendors.hpp"

#include <sl/meta/lifetime/finalizer.hpp>
#include <sl/meta/traits/unique.hpp>

#include <libassert/assert.hpp>

namespace sl::gfx {

enum class framebuffer_usage : GLenum {
    draw = GL_DRAW_FRAMEBUFFER,
    read = GL_READ_FRAMEBUFFER,
    both = GL_FRAMEBUFFER,
};

// TODO: a bit more sane?
enum class framebuffer_attachment {
    color0 = GL_COLOR_ATTACHMENT0,
    color1 = GL_COLOR_ATTACHMENT1,
    color2 = GL_COLOR_ATTACHMENT2,
    color3 = GL_COLOR_ATTACHMENT3,
    color4 = GL_COLOR_ATTACHMENT4,
    color5 = GL_COLOR_ATTACHMENT5,
    color6 = GL_COLOR_ATTACHMENT6,
    color7 = GL_COLOR_ATTACHMENT7,
    color8 = GL_COLOR_ATTACHMENT8,
    color9 = GL_COLOR_ATTACHMENT9,
    color10 = GL_COLOR_ATTACHMENT10,
    color11 = GL_COLOR_ATTACHMENT11,
    color12 = GL_COLOR_ATTACHMENT12,
    color13 = GL_COLOR_ATTACHMENT13,
    color14 = GL_COLOR_ATTACHMENT14,
    color15 = GL_COLOR_ATTACHMENT15,
    color16 = GL_COLOR_ATTACHMENT16,
    color17 = GL_COLOR_ATTACHMENT17,
    color18 = GL_COLOR_ATTACHMENT18,
    color19 = GL_COLOR_ATTACHMENT19,
    color20 = GL_COLOR_ATTACHMENT20,
    color21 = GL_COLOR_ATTACHMENT21,
    color22 = GL_COLOR_ATTACHMENT22,
    color23 = GL_COLOR_ATTACHMENT23,
    color24 = GL_COLOR_ATTACHMENT24,
    color25 = GL_COLOR_ATTACHMENT25,
    color26 = GL_COLOR_ATTACHMENT26,
    color27 = GL_COLOR_ATTACHMENT27,
    color28 = GL_COLOR_ATTACHMENT28,
    color29 = GL_COLOR_ATTACHMENT29,
    color30 = GL_COLOR_ATTACHMENT30,
    color31 = GL_COLOR_ATTACHMENT31,
    depth = GL_DEPTH_ATTACHMENT,
    stencil = GL_STENCIL_ATTACHMENT,
    depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT,
};

template <framebuffer_usage usage>
class bound_framebuffer;

class framebuffer : public meta::finalizer<framebuffer> {
public:
    framebuffer()
        : meta::finalizer<framebuffer>{ [](framebuffer& self) {
              log::trace("glDeleteFramebuffers: {}", self.internal_);
              glDeleteFramebuffers(1, &self.internal_);
          } },
          internal_{ [] {
              GLuint framebuffer = 0;
              // TODO(@usatiynyan): more than one buffer?
              log::trace("glGenFramebuffers: {}", framebuffer);
              glGenFramebuffers(1, &framebuffer);
              return framebuffer;
          }() } {}

    template <framebuffer_usage usage = framebuffer_usage::both>
    [[nodiscard]] bound_framebuffer<usage> bind();

    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    GLuint internal_;
};

template <framebuffer_usage usage>
class bound_framebuffer : public meta::finalizer<bound_framebuffer<usage>> {
public:
    explicit bound_framebuffer(framebuffer& framebuffer)
        : meta::finalizer<bound_framebuffer>{ [](bound_framebuffer&) {
              log::trace("glBindFramebuffer({}): 0", static_cast<GLenum>(usage));
              glBindFramebuffer(GL_FRAMEBUFFER, 0);
          } } {
        log::trace("glBindFramebuffer({}): {}", static_cast<GLenum>(usage), framebuffer.internal());
        glBindFramebuffer(static_cast<GLenum>(usage), framebuffer.internal());
    }

    void attach_texture(GLenum attachment, bound_texture& bound_texture) {
        const auto gl_usage = static_cast<GLenum>(usage);
        const auto tex_type = bound_texture->type();
        const auto gl_tex_type = static_cast<GLenum>(tex_type);
        const auto internal = bound_texture->internal();
        constexpr GLint level = 0; // TODO
        switch (tex_type) {
        case texture_type::texture_1d:
            glFramebufferTexture1D(gl_usage, attachment, gl_tex_type, internal, level);
            break;
        case texture_type::texture_2d:
            glFramebufferTexture2D(gl_usage, attachment, gl_tex_type, internal, level);
            break;
        default:
            PANIC("attachment for texture type is not implemented :(", tex_type);
            break;
        }
    }

    // TODO(@usatiynyan): attach_renderbuffer
};

template <framebuffer_usage usage>
bound_framebuffer<usage> framebuffer::bind() {
    return bound_framebuffer<usage>{ *this };
}

} // namespace sl::gfx
