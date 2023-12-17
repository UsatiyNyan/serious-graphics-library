//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/type_map.hpp"
#include "sl/gfx/vtx/buffer.hpp"

#include "sl/meta/lifetime/finalizer.hpp"

#include <boost/container/static_vector.hpp>
#include <tl/optional.hpp>

namespace sl::gfx {

struct VertexArrayAttribute {
    GLuint index;
    GLint components_count;
    GLenum type;
    GLsizei offset;
    GLsizei stride;
    bool normalized;
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
    // TODO(@usatiynyan): retrieve in configuration glGetIntegerv(GL_MAX_VERTEX_ATTRIBS)
    static constexpr std::size_t MAX_ATTRIBS = 16;

public:
    template <GLint component_count, typename T, bool normalized = false>
    void attribute() {
        static_assert(1 <= component_count && component_count <= 4, "components per vertex not in range [1..4]");
        attribute(VertexArrayAttribute{
            .index = attribs_.size(),
            .components_count = component_count,
            .type = type_map<T>::value,
            .offset = accumulated_stride_,
            .stride = component_count * sizeof(T),
            .normalized = normalized,
        });
    }

    template <BufferType buffer_type, BufferUsage buffer_usage, typename DataType, std::size_t extent>
    [[nodiscard]] auto buffer(std::span<const DataType, extent> data) {
        Buffer<DataType, buffer_type, buffer_usage> buffer;
        buffer.bind().set_data(data);
        return buffer;
    }

    VertexArray submit() &&;
private:
    void attribute(VertexArrayAttribute va_attr) {
        accumulated_stride_ += va_attr.stride;
        attribs_.push_back(va_attr);
    }

private:
    tl::optional<VertexArray> va_{ VertexArray{} };
    tl::optional<VertexArray::Bind> bind_{ tl::in_place, *va_ };
    GLsizei accumulated_stride_ = 0;
    boost::container::static_vector<VertexArrayAttribute, MAX_ATTRIBS> attribs_{};
};

} // namespace sl::gfx
