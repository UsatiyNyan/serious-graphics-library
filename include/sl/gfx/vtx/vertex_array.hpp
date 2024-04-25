//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/primitives/gl_type_map.hpp"
#include "sl/gfx/vtx/buffer.hpp"
#include "sl/gfx/vtx/vertex_array_attribute.hpp"

#include <sl/meta/lifetime/finalizer.hpp>
#include <sl/meta/lifetime/unique.hpp>
#include <sl/meta/tuple/decay.hpp>
#include <sl/meta/tuple/tie_as_tuple.hpp>

#include <boost/container/static_vector.hpp>
#include <tl/optional.hpp>

namespace sl::gfx {

class bound_vertex_array;

class vertex_array : public meta::finalizer<vertex_array> {
public:
    vertex_array();

    [[nodiscard]] bound_vertex_array bind() const;

    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    GLuint internal_;
};

class bound_vertex_array : public meta::finalizer<bound_vertex_array> {
public:
    explicit bound_vertex_array(const vertex_array& va);
};

class vertex_array_builder : public meta::unique {
    template <typename = void>
    struct extract_attribute;

    template <GLint component_count, typename T, typename AlignAsT, bool normalized>
    struct extract_attribute<va_attrib_field<component_count, T, AlignAsT, normalized>> {
        void operator()(vertex_array_builder& va_builder) {
            va_builder.attribute(vertex_array_attribute{
                .index = static_cast<GLuint>(va_builder.attributes_.size()),
                .components_count = component_count,
                .type = gl_type_query<T>,
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
        meta::for_each_type<extract_attribute, decayed_tied_tuple>(*this);
    }

    template <GLint component_count, typename T, bool normalized = false>
        requires is_allowed_component_count<component_count>
    void attribute() {
        attribute(vertex_array_attribute{
            .index = static_cast<GLuint>(attributes_.size()),
            .components_count = component_count,
            .type = gl_type_query<T>,
            .offset = accumulated_stride_,
            .stride = component_count * sizeof(T),
            .normalized = normalized,
        });
    }

    template <buffer_type type, buffer_usage usage, typename T, std::size_t extent>
        requires(type == buffer_type::array || type == buffer_type::element_array)
    [[nodiscard]] auto buffer(std::span<const T, extent> data) {
        gfx::buffer<T, type, usage> buffer;
        buffer.bind().set_data(data);
        return buffer;
    }

    vertex_array submit() &&;

private:
    void attribute(vertex_array_attribute va_attribute);

private:
    tl::optional<vertex_array> va_{ vertex_array{} };
    tl::optional<bound_vertex_array> bound_{ tl::in_place, *va_ };
    GLsizei accumulated_stride_ = 0;
    boost::container::static_vector<vertex_array_attribute, max_vertex_array_attributes> attributes_{};
};

} // namespace sl::gfx
