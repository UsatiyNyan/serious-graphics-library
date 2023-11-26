//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/common/log.hpp"

#include "log_detail.hpp"
#include "parameter_detail.hpp"

namespace sl::gfx {
ShaderProgram::ShaderProgram()
    : finalizer{ [](ShaderProgram& self) { glDeleteProgram(self.program_); } }, program_{ glCreateProgram() } {}

void ShaderProgram::use() const { glUseProgram(program_); }

tl::optional<GLint> ShaderProgram::get_uniform_location(std::string_view uniform_name) const {
    GLint uniform_location = glGetUniformLocation(program_, uniform_name.data());
    if (uniform_location == -1) {
        log::error("uniform {} location not found", uniform_name);
        return tl::nullopt;
    }
    return uniform_location;
}

GLint ShaderProgram::get_parameter(GLenum parameter_name) const {
    return detail::get_parameter(glGetProgramiv, program_, parameter_name);
}

std::string ShaderProgram::get_log() const { return detail::get_log(glGetProgramInfoLog, program_); }

void ShaderProgram::attach(const Shader& shader) { glAttachShader(program_, *shader); }

void ShaderProgram::link() {
    glLinkProgram(program_);
    if (!get_parameter(GL_LINK_STATUS)) {
        log::error("Program link failed {}", get_log());
    }
}
} // namespace sl::gfx
