//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/gfx/shader/shader.hpp"

#include "sl/meta/lifetime/finalizer.hpp"
#include "sl/gfx/common/vendors.hpp"

#include <range/v3/view/enumerate.hpp>
#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {
class ShaderProgram : public finalizer<ShaderProgram> {
public:
    class Bind {
    public:
        explicit Bind(const ShaderProgram& sp);

        template <typename UniformSetter>
        [[nodiscard]] auto make_uniform_setter(UniformSetter uniform_setter, std::string_view uniform_name) const {
            return get_uniform_location(uniform_name)
                .map([sp_object = object_, uniform_setter](GLint uniform_location) {
                    // up to 4 args
                    return [sp_object, uniform_setter, uniform_location](const Bind& sp_bind, auto&&... args) {
                        sp_bind.verify_bound(sp_object);
                        return uniform_setter(uniform_location, args...);
                    };
                });
        }

        template <typename UniformvSetter>
        [[nodiscard]] auto
            make_uniform_v_setter(UniformvSetter uniform_v_setter, std::string_view uniform_name, GLsizei count) const {
            return get_uniform_location(uniform_name)
                .map([sp_object = object_, uniform_v_setter, count](GLint uniform_location) {
                    return [sp_object, uniform_v_setter, uniform_location, count] //
                        (const Bind& sp_bind, const auto* values) {
                            sp_bind.verify_bound(sp_object);
                            return uniform_v_setter(uniform_location, count, values);
                        };
                });
        }

        template <typename UniformvMatrixSetter>
        [[nodiscard]] auto make_uniform_matrix_v_setter(
            UniformvMatrixSetter uniform_matrix_v_setter,
            std::string_view uniform_name,
            GLsizei count,
            bool transpose
        ) const {
            return get_uniform_location(uniform_name)
                .map([sp_object = object_, uniform_matrix_v_setter, count, transpose](GLint uniform_location) {
                    return [sp_object, uniform_matrix_v_setter, uniform_location, count, transpose] //
                        (const Bind& sp_bind, const auto* values) {
                            sp_bind.verify_bound(sp_object);
                            return uniform_matrix_v_setter(uniform_location, count, transpose, values);
                        };
                });
        }

        template <std::size_t extent>
        void initialize_tex_units(std::span<std::string_view, extent> tex_uniform_names) {
            for (const auto& [i, tex_uniform_name] : ranges::views::enumerate(tex_uniform_names)) {
                initialize_tex_unit(tex_uniform_name, i);
            }
        }

    private:
        [[nodiscard]] tl::optional<GLint> get_uniform_location(std::string_view uniform_name) const;
        void verify_bound(GLuint sp_object) const;

        void initialize_tex_unit(std::string_view tex_uniform_name, std::size_t tex_unit);

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
