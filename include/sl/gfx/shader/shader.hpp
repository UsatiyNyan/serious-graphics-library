//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

#include <sl/meta/lifetime/finalizer.hpp>

#include <tl/optional.hpp>

#include <filesystem>
#include <string_view>

namespace sl::gfx {
enum class ShaderType : GLenum {
    COMPUTE = GL_COMPUTE_SHADER,
    VERTEX = GL_VERTEX_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
};

class Shader : public meta::finalizer<Shader> {
public:
    static tl::optional<Shader> load_from_file(ShaderType shader_type, const std::filesystem::path& shader_path);
    static tl::optional<Shader> load_from_source(ShaderType shader_type, std::string_view shader_source);

private:
    explicit Shader(ShaderType shader_type);

public:
    [[nodiscard]] GLint get_parameter(GLenum parameter_name) const;
    [[nodiscard]] std::string get_log() const;

    [[nodiscard]] GLuint operator*() const { return object_; }

private:
    GLuint object_;
};
} // namespace sl::gfx
