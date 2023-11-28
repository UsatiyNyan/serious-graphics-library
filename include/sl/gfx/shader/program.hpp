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
public:
    class Bind {
    public:
        explicit Bind(const ShaderProgram& sp);

        template <typename UniformSetter>
        [[nodiscard]] auto make_uniform_setter(std::string_view uniform_name, UniformSetter uniform_setter) const {
            return get_uniform_location(uniform_name)
                .map([uniform_setter, sp_object = object_](GLint uniform_location) {
                    return [uniform_setter, sp_object, uniform_location](const Bind& sp_bind, auto&&... args) {
                        sp_bind.verify_bound(sp_object);
                        return uniform_setter(uniform_location, args...);
                    };
                });
        }

    private:
        [[nodiscard]] tl::optional<GLint> get_uniform_location(std::string_view uniform_name) const;
        void verify_bound(GLuint sp_object) const;

    private:
        GLuint object_;
    };

private:
    ShaderProgram();

public:
    template <std::size_t extent>
    explicit ShaderProgram(std::span<const Shader, extent> shaders) : ShaderProgram{} {
        for (const Shader& shader : shaders) {
            attach(shader);
        }
        link();
    }

    [[nodiscard]] GLuint operator*() const { return object_; }
    [[nodiscard]] auto bind() const { return Bind{ *this }; }

    [[nodiscard]] GLint get_parameter(GLenum parameter_name) const;
    [[nodiscard]] std::string get_log() const;

private:
    void attach(const Shader& shader);
    void link();

private:
    GLuint object_{};
};

} // namespace sl::gfx
