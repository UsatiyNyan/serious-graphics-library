//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx/shader/shader.hpp"
#include "sl/gfx/common/log.hpp"

#include "log_detail.hpp"
#include "parameter_detail.hpp"

#include <fstream>

namespace sl::gfx {
tl::optional<Shader> Shader::load_from_file(ShaderType shader_type, const std::filesystem::path& shader_path) {
    std::ifstream shader_file{ shader_path };
    const std::string shader_source{ std::istreambuf_iterator<char>{ shader_file }, std::istreambuf_iterator<char>{} };
    return load_from_source(shader_type, std::string_view{ shader_source });
}

tl::optional<Shader> Shader::load_from_source(ShaderType shader_type, std::string_view shader_source) {
    Shader shader{ shader_type };

    // only one source is used since it is redundant to send multiple shader sources
    log::debug("glShaderSource: {}", shader_source);
    const char* source_data = shader_source.data();
    const auto source_length = static_cast<GLint>(shader_source.length());
    glShaderSource(*shader, 1, &source_data, &source_length);

    log::debug("glCompileShader: {}", *shader);
    glCompileShader(*shader);
    if (shader.get_parameter(GL_COMPILE_STATUS) == GL_FALSE) {
        log::error("Object compilation failed {}", shader.get_log());
        return tl::nullopt;
    }

    return shader;
}

Shader::Shader(ShaderType shader_type)
    : finalizer{ [](Shader& self) {
          log::debug("glDeleteShader: {}", *self);
          glDeleteShader(*self);
      } },
      object_{ glCreateShader(static_cast<GLenum>(shader_type)) } {
    log::debug("glCreateShader: {}", object_);
}

GLint Shader::get_parameter(GLenum parameter_name) const {
    return detail::get_parameter(glGetShaderiv, object_, parameter_name);
}

std::string Shader::get_log() const { return detail::get_log(glGetShaderInfoLog, object_); }
} // namespace sl::gfx
