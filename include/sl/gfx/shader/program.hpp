//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/gfx/shader/shader.hpp"

#include "sl/gfx/detail/vendors.hpp"

#include <glm/detail/qualifier.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sl/meta/lifetime/finalizer.hpp>
#include <sl/meta/monad/maybe.hpp>
#include <sl/meta/traits/unique.hpp>

#include <libassert/assert.hpp>
#include <range/v3/view/enumerate.hpp>
#include <span>

namespace sl::gfx {
namespace detail {

template <glm::qualifier Q>
constexpr bool glm_qualifier_is_packed = Q == glm::packed_highp || Q == glm::packed_mediump || Q == glm::packed_lowp;

} // namespace detail

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

template <typename UniformSetterT>
struct [[nodiscard]] uniform_setter;

template <typename UniformVSetterT>
struct [[nodiscard]] uniform_v_setter;

template <typename UniformVMatrixSetterT>
struct [[nodiscard]] uniform_matrix_v_setter;

class bound_shader_program : public meta::unique {
public:
    explicit bound_shader_program(const shader_program& sp);

    template <typename UniformSetterT>
    meta::maybe<uniform_setter<UniformSetterT>> make_uniform_setter(UniformSetterT impl, std::string_view name) const;

    template <typename UniformVSetterT>
    meta::maybe<uniform_v_setter<UniformVSetterT>>
        make_uniform_v_setter(UniformVSetterT impl, std::string_view name, GLsizei count) const;

    template <typename UniformVMatrixSetterT>
    meta::maybe<uniform_matrix_v_setter<UniformVMatrixSetterT>>
        make_uniform_matrix_v_setter(UniformVMatrixSetterT impl, std::string_view name, GLsizei count, bool transpose)
            const;

    template <std::size_t extent>
    void initialize_tex_units(std::span<const std::string_view, extent> tex_uniform_names) {
        for (const auto& [i, tex_uniform_name] : ranges::views::enumerate(tex_uniform_names)) {
            initialize_tex_unit(tex_uniform_name, i);
        }
    }

    void verify(GLuint internal) const { ASSERT(sp_.internal() == internal); }

private:
    [[nodiscard]] tl::optional<GLint> get_uniform_location(std::string_view uniform_name) const;

    void initialize_tex_unit(std::string_view tex_uniform_name, std::size_t tex_unit);

private:
    const shader_program& sp_;
};

template <typename UniformSetterT>
struct [[nodiscard]] uniform_setter {
    uniform_setter(GLuint shader, UniformSetterT impl, GLint location)
        : impl_{ impl }, location_{ location }, shader_{ shader } {}

    template <glm::length_t L, typename T, glm::qualifier Q>
    auto operator()(const bound_shader_program& bound_sp, const glm::vec<L, T, Q>& vec) const& {
        return variadic_impl<L, T, Q>(bound_sp, vec, std::make_index_sequence<L>());
    }

    auto operator()(const bound_shader_program& bound_sp, auto&&... args) const& {
        bound_sp.verify(shader_);
        return impl_(location_, args...);
    }

private:
    template <glm::length_t L, typename T, glm::qualifier Q, std::size_t... Idxs>
    auto variadic_impl(const bound_shader_program& bound_sp, const glm::vec<L, T, Q>& vec, std::index_sequence<Idxs...>)
        const& {
        return operator()(bound_sp, vec[Idxs]...);
    }

private:
    UniformSetterT impl_;
    GLint location_;
    GLuint shader_;
};

template <typename UniformSetterT>
meta::maybe<uniform_setter<UniformSetterT>>
    bound_shader_program::make_uniform_setter(UniformSetterT impl, std::string_view name) const {
    return get_uniform_location(name).map([this, impl](GLint location) {
        return uniform_setter<UniformSetterT>{ sp_.internal(), impl, location };
    });
}

template <typename UniformVSetterT>
struct [[nodiscard]] uniform_v_setter {
    uniform_v_setter(GLuint shader, UniformVSetterT impl, GLint location, GLsizei count)
        : impl_{ impl }, location_{ location }, count_{ count }, shader_{ shader } {}

    auto operator()(const bound_shader_program& bound_sp, const auto* values) const& {
        bound_sp.verify(shader_);
        return impl_(location_, count_, values);
    }

    template <glm::length_t L, typename T, glm::qualifier Q>
        requires(detail::glm_qualifier_is_packed<Q>)
    auto operator()(const bound_shader_program& bound_sp, const glm::vec<L, T, Q>& vec) const& {
        ASSERT(count_ == 1);
        return operator()(bound_sp, glm::value_ptr(vec));
    }

    template <glm::length_t L, typename T, glm::qualifier Q, std::size_t extent = std::dynamic_extent>
        requires(detail::glm_qualifier_is_packed<Q>)
    auto operator()(const bound_shader_program& bound_sp, std::span<const glm::vec<L, T, Q>, extent> vecs) const& {
        ASSERT(count_ == vecs.size());
        return operator()(bound_sp, glm::value_ptr(vecs.data()));
    }

private:
    UniformVSetterT impl_;
    GLint location_;
    GLsizei count_;
    GLuint shader_;
};

template <typename UniformVSetterT>
meta::maybe<uniform_v_setter<UniformVSetterT>>
    bound_shader_program::make_uniform_v_setter(UniformVSetterT impl, std::string_view name, GLsizei count) const {
    return get_uniform_location(name).map([this, impl, count](GLint location) {
        return uniform_v_setter<UniformVSetterT>{ sp_.internal(), impl, location, count };
    });
}

template <typename UniformVMatrixSetterT>
struct [[nodiscard]] uniform_matrix_v_setter {
    uniform_matrix_v_setter(GLuint shader, UniformVMatrixSetterT impl, GLint location, GLsizei count, bool transpose)
        : impl_{ impl }, location_{ location }, count_{ count }, shader_{ shader }, transpose_{ transpose } {}

    auto operator()(const bound_shader_program& bound_sp, const auto* values) const& {
        bound_sp.verify(shader_);
        return impl_(location_, count_, transpose_, values);
    };

    template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
        requires(detail::glm_qualifier_is_packed<Q>)
    auto operator()(const bound_shader_program& bound_sp, const glm::mat<C, R, T, Q>& mat) const& {
        ASSERT(count_ == 1);
        return operator()(bound_sp, glm::value_ptr(mat));
    }

    template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q, std::size_t extent = std::dynamic_extent>
        requires(detail::glm_qualifier_is_packed<Q>)
    auto operator()(const bound_shader_program& bound_sp, std::span<const glm::mat<C, R, T, Q>, extent> mats) const& {
        ASSERT(count_ == mats.size());
        return operator()(bound_sp, glm::value_ptr(mats.data()));
    }

private:
    UniformVMatrixSetterT impl_;
    GLint location_;
    GLsizei count_;
    GLuint shader_;
    bool transpose_;
};

template <typename UniformVMatrixSetterT>
meta::maybe<uniform_matrix_v_setter<UniformVMatrixSetterT>> bound_shader_program::make_uniform_matrix_v_setter(
    UniformVMatrixSetterT impl,
    std::string_view name,
    GLsizei count,
    bool transpose
) const {
    return get_uniform_location(name).map([this, impl, count, transpose](GLint location) {
        return uniform_matrix_v_setter<UniformVMatrixSetterT>{ sp_.internal(), impl, location, count, transpose };
    });
}
} // namespace sl::gfx
