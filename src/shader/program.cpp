//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/common/log.hpp"

#include "sl/gfx/detail/log.hpp"
#include "sl/gfx/detail/parameter.hpp"

#include <libassert/assert.hpp>

namespace sl::gfx {
shader_program::shader_program()
    : finalizer{ [](shader_program& self) {
          LOG_DEBUG("glDeleteProgram: {}", self.internal_);
          glDeleteProgram(self.internal_);
      } },
      internal_{ glCreateProgram() } {
    LOG_DEBUG("glCreateProgram: {}", internal_);
}

bound_shader_program shader_program::bind() const { return bound_shader_program{ *this }; }

GLint shader_program::get_parameter(GLenum parameter_name) const {
    return detail::get_parameter(glGetProgramiv, internal_, parameter_name);
}

std::string shader_program::get_log() const { return detail::get_log(glGetProgramInfoLog, internal_); }

void shader_program::attach(const shader& shader) { glAttachShader(internal_, shader.internal()); }

bool shader_program::link() {
    glLinkProgram(internal_);
    const bool link_status = get_parameter(GL_LINK_STATUS);
    if (!link_status) {
        log::error("Program link failed {}", get_log());
    }
    return link_status;
}

bound_shader_program::bound_shader_program(const shader_program& sp) : sp_{ sp } {
    LOG_DEBUG("glUseProgram: {}", sp_.internal());
    glUseProgram(sp_.internal());
}

tl::optional<GLint> bound_shader_program::get_uniform_location(std::string_view uniform_name) const {
    GLint uniform_location = glGetUniformLocation(sp_.internal(), uniform_name.data());
    if (uniform_location == -1) {
        log::error("uniform {} location not found", uniform_name);
        return tl::nullopt;
    }
    LOG_DEBUG("glGetUniformLocation({}, {}): {}", sp_.internal(), uniform_name, uniform_location);
    return uniform_location;
}

void bound_shader_program::verify(GLuint internal) const { ASSERT(sp_.internal() == internal); }

void bound_shader_program::initialize_tex_unit(std::string_view tex_uniform_name, std::size_t tex_unit) {
    const auto set_texture_unit = *ASSERT_VAL(make_uniform_setter(glUniform1i, tex_uniform_name));
    set_texture_unit(*this, tex_unit);
}

} // namespace sl::gfx
