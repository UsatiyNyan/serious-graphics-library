//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/type_map.hpp"
#include "sl/gfx/vtx/buffer.hpp"
#include "sl/gfx/vtx/vertex_array_attribute.hpp"

#include <sl/meta/lifetime/finalizer.hpp>

#include <boost/container/static_vector.hpp>
#include <tl/optional.hpp>

namespace sl::gfx {

class VertexArray : public meta::finalizer<VertexArray> {
    friend class VertexArrayBuilder;
    VertexArray();

public:
    class Bind : public meta::finalizer<Bind> {
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
private:
    template <typename = void>
    struct ExtractAttribute;

    template <GLint component_count, typename T, typename AlignAsT, bool normalized>
    struct ExtractAttribute<va_attrib_field<component_count, T, AlignAsT, normalized>> {
        void operator()(VertexArrayBuilder& va_builder) {
            va_builder.attribute(VertexArrayAttribute{
                .index = static_cast<GLuint>(va_builder.attribs_.size()),
                .components_count = component_count,
                .type = type_map<T>::value,
                .offset = va_builder.accumulated_stride_,
                .stride = sizeof(va_attrib_field<component_count, T, AlignAsT, normalized>),
                .normalized = normalized,
            });
        }
    };

public:
    template <typename AggregateT>
        requires meta::is_tightly_packed<AggregateT>
    void attributes_from() {
        using decayed_tied_tuple = meta::decay_tuple_t<meta::tie_as_tuple_t<AggregateT>>;
        meta::for_each_type<ExtractAttribute, decayed_tied_tuple>(*this);
    }

    template <GLint component_count, typename T, bool normalized = false>
        requires is_allowed_component_count<component_count>
    void attribute() {
        attribute(VertexArrayAttribute{
            .index = static_cast<GLuint>(attribs_.size()),
            .components_count = component_count,
            .type = type_map<T>::value,
            .offset = accumulated_stride_,
            .stride = component_count * sizeof(T),
            .normalized = normalized,
        });
    }

    template <BufferType buffer_type, BufferUsage buffer_usage, typename DataType, std::size_t extent>
        requires(buffer_type == BufferType::ARRAY || buffer_type == BufferType::ELEMENT_ARRAY)
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
    boost::container::static_vector<VertexArrayAttribute, VertexArrayAttribute::MAX_ATTRIBS> attribs_{};
};

} // namespace sl::gfx
