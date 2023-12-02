//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/common/log.hpp"

#include "sl/gfx/detail/log.hpp"
#include "sl/gfx/detail/parameter.hpp"

#include <assert.hpp>

namespace sl::gfx {
ShaderProgram::Bind::Bind(const ShaderProgram& sp) : object_{ *sp } {
    LOG_DEBUG("glUseProgram: {}", object_);
    glUseProgram(object_);
}

tl::optional<GLint> ShaderProgram::Bind::get_uniform_location(std::string_view uniform_name) const {
    GLint uniform_location = glGetUniformLocation(object_, uniform_name.data());
    if (uniform_location == -1) {
        log::error("uniform {} location not found", uniform_name);
        return tl::nullopt;
    }
    LOG_DEBUG("glGetUniformLocation({}, {}): {}", object_, uniform_name, uniform_location);
    return uniform_location;
}

void ShaderProgram::Bind::verify_bound(GLuint sp_object) const { ASSERT(sp_object == object_); }

ShaderProgram::ShaderProgram()
    : finalizer{ [](ShaderProgram& self) {
          LOG_DEBUG("glDeleteProgram: {}", *self);
          glDeleteProgram(*self);
      } },
      object_{ glCreateProgram() } {
    LOG_DEBUG("glCreateProgram: {}", object_);
}

GLint ShaderProgram::get_parameter(GLenum parameter_name) const {
    return detail::get_parameter(glGetProgramiv, object_, parameter_name);
}

std::string ShaderProgram::get_log() const { return detail::get_log(glGetProgramInfoLog, object_); }

void ShaderProgram::attach(const Shader& shader) { glAttachShader(object_, *shader); }

void ShaderProgram::link() {
    glLinkProgram(object_);
    if (!get_parameter(GL_LINK_STATUS)) {
        log::error("Program link failed {}", get_log());
    }
}
} // namespace sl::gfx
