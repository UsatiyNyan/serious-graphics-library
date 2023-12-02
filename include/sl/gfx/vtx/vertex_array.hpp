//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/finalizer.hpp"
#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/type_map.hpp"
#include "sl/gfx/vtx/buffer.hpp"

#include <boost/container/static_vector.hpp>
#include <tl/optional.hpp>

namespace sl::gfx {

class VertexArrayAttribute {
    constexpr VertexArrayAttribute(
        GLuint index,
        GLint components_size,
        GLenum type,
        GLsizei type_size,
        bool normalized,
        GLsizei offset
    )
        : index_{ index }, components_size_{ components_size }, type_{ type }, normalized_{ normalized },
          offset_{ offset }, stride_{ components_size * type_size } {}

public:
    // TODO(@usatiynyan): retrieve in configuration glGetIntegerv(GL_MAX_VERTEX_ATTRIBS)
    static constexpr std::size_t MAX_AMOUNT = 16;

    template <GLint ComponentCount, typename T>
    static constexpr VertexArrayAttribute make(GLuint index, bool normalized, GLsizei offset) {
        static_assert(1 <= ComponentCount && ComponentCount <= 4, "components per vertex not in range [1..4]");
        return VertexArrayAttribute{ index, ComponentCount, type_map<T>::value, sizeof(T), normalized, offset };
    }

    [[nodiscard]] constexpr GLsizei stride() const { return stride_; }

    void set_pointer(GLsizei accumulated_stride);
    void enable();

private:
    GLuint index_;
    GLint components_size_;
    GLenum type_;
    bool normalized_;
    GLsizei offset_;
    GLsizei stride_;
};

class VertexArray : public finalizer<VertexArray> {
    friend class VertexArrayBuilder;
    VertexArray();

public:
    class Bind : public finalizer<Bind> {
    public:
        explicit Bind(const VertexArray& va);
    };

public:
    [[nodiscard]] GLuint operator*() const { return object_; }
    [[nodiscard]] auto bind() const { return Bind{ *this }; }

private:
    GLuint object_;
};

class VertexArrayBuilder {
public:
    template <GLint ComponentCount, typename T>
    void attribute(bool normalized = false) {
        auto attribute =
            VertexArrayAttribute::make<ComponentCount, T>(attributes_.size(), normalized, accumulated_stride_);
        const GLsizei attribute_stride = attribute.stride();
        attributes_.push_back(std::move(attribute));
        accumulated_stride_ += attribute_stride;
    }

    template <BufferType buffer_type, BufferUsage buffer_usage, typename DataType, std::size_t extent>
    [[nodiscard]] auto buffer(std::span<const DataType, extent> data) {
        Buffer<DataType, buffer_type, buffer_usage> buffer;
        buffer.bind().set_data(data);
        return buffer;
    }

    VertexArray submit() &&;

private:
    tl::optional<VertexArray> va_{ VertexArray{} };
    tl::optional<VertexArray::Bind> bind_{ VertexArray::Bind{ *va_ } };
    GLsizei accumulated_stride_ = 0;
    boost::container::static_vector<VertexArrayAttribute, VertexArrayAttribute::MAX_AMOUNT> attributes_{};
};

} // namespace sl::gfx
