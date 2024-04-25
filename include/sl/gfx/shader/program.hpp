//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/shader/shader.hpp"

#include <sl/meta/lifetime/finalizer.hpp>
#include <sl/meta/lifetime/unique.hpp>

#include <range/v3/view/enumerate.hpp>
#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {

class bound_shader_program;

class shader_program : public meta::finalizer<shader_program> {
    shader_program();

public:
    template <std::size_t extent>
    static tl::optional<shader_program> build(std::span<const shader, extent> shaders) {
        shader_program sp;
        for (const shader& shader : shaders) {
            sp.attach(shader);
        }
        if (!sp.link()) {
            return tl::nullopt;
        }
        return sp;
    }

    [[nodiscard]] bound_shader_program bind() const;

    [[nodiscard]] GLint get_parameter(GLenum parameter_name) const;
    [[nodiscard]] std::string get_log() const;

    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    void attach(const shader& shader);
    bool link();

private:
    GLuint internal_;
};

class bound_shader_program : public meta::unique {
public:
    explicit bound_shader_program(const shader_program& sp);

    template <typename UniformSetter>
    [[nodiscard]] auto make_uniform_setter(UniformSetter uniform_setter, std::string_view uniform_name) const {
        return get_uniform_location(uniform_name)
            .map([internal = sp_.internal(), uniform_setter](GLint uniform_location) {
                // up to 4 args
                return
                    [internal, uniform_setter, uniform_location](const bound_shader_program& bound_sp, auto&&... args) {
                        bound_sp.verify(internal);
                        return uniform_setter(uniform_location, args...);
                    };
            });
    }

    template <typename UniformvSetter>
    [[nodiscard]] auto
        make_uniform_v_setter(UniformvSetter uniform_v_setter, std::string_view uniform_name, GLsizei count) const {
        return get_uniform_location(uniform_name)
            .map([internal = sp_.internal(), uniform_v_setter, count](GLint uniform_location) {
                return [internal, uniform_v_setter, uniform_location, count] //
                    (const bound_shader_program& bound_sp, const auto* values) {
                        bound_sp.verify(internal);
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
            .map([internal = sp_.internal(), uniform_matrix_v_setter, count, transpose](GLint uniform_location) {
                return [internal, uniform_matrix_v_setter, uniform_location, count, transpose] //
                    (const bound_shader_program& bound_sp, const auto* values) {
                        bound_sp.verify(internal);
                        return uniform_matrix_v_setter(uniform_location, count, transpose, values);
                    };
            });
    }

    template <std::size_t extent>
    void initialize_tex_units(std::span<const std::string_view, extent> tex_uniform_names) {
        for (const auto& [i, tex_uniform_name] : ranges::views::enumerate(tex_uniform_names)) {
            initialize_tex_unit(tex_uniform_name, i);
        }
    }

private:
    [[nodiscard]] tl::optional<GLint> get_uniform_location(std::string_view uniform_name) const;
    void verify(GLuint internal) const;

    void initialize_tex_unit(std::string_view tex_uniform_name, std::size_t tex_unit);

private:
    const shader_program& sp_;
};

} // namespace sl::gfx
