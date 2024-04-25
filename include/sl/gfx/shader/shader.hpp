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
enum class shader_type : GLenum {
    compute = GL_COMPUTE_SHADER,
    vertex = GL_VERTEX_SHADER,
    geometry = GL_GEOMETRY_SHADER,
    fragment = GL_FRAGMENT_SHADER,
};

class shader : public meta::finalizer<shader> {
public:
    static tl::optional<shader> load_from_file(shader_type type, const std::filesystem::path& path);
    static tl::optional<shader> load_from_source(shader_type type, std::string_view source);

private:
    explicit shader(shader_type type);

public:
    [[nodiscard]] GLint get_parameter(GLenum parameter_name) const;
    [[nodiscard]] std::string get_log() const;

    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    GLuint internal_;
};
} // namespace sl::gfx
