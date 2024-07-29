//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx/shader/shader.hpp"

#include "sl/gfx/detail/get.hpp"
#include "sl/gfx/detail/log.hpp"

#include <fstream>

namespace sl::gfx {

tl::optional<shader> shader::load_from_file(shader_type type, const std::filesystem::path& path) {
    std::ifstream file{ path };
    using istream_it = std::istreambuf_iterator<char>;
    const std::string source{ istream_it{ file }, istream_it{} };
    return load_from_source(type, std::string_view{ source });
}

tl::optional<shader> shader::load_from_source(shader_type type, std::string_view source) {
    if (source.empty()) {
        log::error("Shader source is empty");
        return tl::nullopt;
    }

    shader shader{ type };

    // only one source is used since it is redundant to send multiple shader sources
    log::trace("glShaderSource: {}", source);
    const char* source_data = source.data();
    const auto source_length = static_cast<GLint>(source.length());
    glShaderSource(shader.internal(), 1, &source_data, &source_length);

    log::trace("glCompileShader: {}", shader.internal());
    glCompileShader(shader.internal());
    if (shader.get_parameter(GL_COMPILE_STATUS) == GL_FALSE) {
        log::error("Object compilation failed {}", shader.get_log());
        return tl::nullopt;
    }

    return shader;
}

shader::shader(shader_type type)
    : finalizer{ [](shader& self) {
          log::trace("glDeleteShader: {}", self.internal_);
          glDeleteShader(self.internal_);
      } },
      internal_{ glCreateShader(static_cast<GLenum>(type)) } {
    log::trace("glCreateShader: {}", internal_);
}

GLint shader::get_parameter(GLenum parameter_name) const {
    return detail::get_parameter(glGetShaderiv, internal_, parameter_name);
}

std::string shader::get_log() const { return detail::get_log(glGetShaderInfoLog, internal_); }

} // namespace sl::gfx
