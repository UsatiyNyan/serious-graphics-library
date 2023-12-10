//
// Created by usatiynyan on 12/10/23.
//

#pragma once

#include "sl/gfx/shader/program.hpp"
#include "sl/gfx/vtx/vertex_array.hpp"
#include "sl/gfx/common/vendors.hpp"

#include <sl/meta/tuple/for_each_meta_enumerate.hpp>

namespace sl::gfx {

namespace detail {

template <std::size_t unit_>
struct TextureActivate {
    template <TextureType type_>
    [[nodiscard]] constexpr auto operator()(const Texture<type_>& tex) {
        return tex.template activate<unit_>();
    }
};

} // namespace detail

template <TextureType... tex_types>
class Draw {
public:
    Draw(const ShaderProgram& sp, const VertexArray& va, const std::tuple<Texture<tex_types>...>& texs)
        : sp_bind_{ sp }, va_bind_{ va },
          tex_binds_{ meta::for_each_meta_enumerate<detail::TextureActivate>(texs) } {}

    [[nodiscard]] const auto& sp_bind() const { return sp_bind_; }
    [[nodiscard]] const auto& va_bind() const { return va_bind_; }
    [[nodiscard]] const auto& tex_binds() const { return tex_binds_; }

    template <typename DataType>
    void elements(const Buffer<DataType, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>& eb) {
        glDrawElements(GL_TRIANGLES, eb.data_size(), type_map<DataType>::value, nullptr);
    }

private:
    ShaderProgram::Bind sp_bind_;
    VertexArray::Bind va_bind_;
    std::tuple<typename Texture<tex_types>::ConstBind...> tex_binds_;
};

} // namespace sl::gfx

