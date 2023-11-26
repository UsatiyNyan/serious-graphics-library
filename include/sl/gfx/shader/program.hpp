//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/gfx/shader/shader.hpp"

#include "sl/gfx/common/finalizer.hpp"
#include "sl/gfx/common/vendors.hpp"

#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {
class ShaderProgram : public finalizer<ShaderProgram> {
    ShaderProgram();

public:
    template <std::size_t extent>
    explicit ShaderProgram(std::span<const Shader, extent> shaders) : ShaderProgram{} {
        for (const Shader& shader : shaders) {
            attach(shader);
        }
        link();
    }

    void use() const;

    [[nodiscard]] tl::optional<GLint> get_uniform_location(std::string_view uniform_name) const;
    [[nodiscard]] GLint get_parameter(GLenum parameter_name) const;
    [[nodiscard]] std::string get_log() const;

    GLuint operator*() const { return program_; }

    template <typename UniformSetter>
    auto make_uniform_setter(std::string_view uniform_name, UniformSetter&& uniform_setter) const {
        return get_uniform_location(uniform_name) //
            .map([uniform_setter = std::forward<UniformSetter>(uniform_setter)](GLint uniform_location) {
                return [uniform_setter = std::forward<UniformSetter>(uniform_setter),
                        uniform_location](auto&&... args) { return uniform_setter(uniform_location, args...); };
            });
    }

private:
    void attach(const Shader& shader);
    void link();

private:
    GLuint program_{};
};

} // namespace sl::gfx
