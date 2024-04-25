//
// Created by usatiynyan on 11/26/23.
//

#include "sl/gfx/vtx/vertex_array.hpp"

#include "sl/gfx/common/log.hpp"

#include <libassert/assert.hpp>

namespace sl::gfx {
namespace {

void vertex_attrib_pointer(const vertex_array_attribute& attribute, GLsizei accumulated_stride) {
    LOG_DEBUG(
        "glVertexAttribPointer: index={} size={} type={} normalized={} stride={} pointer={}",
        attribute.index,
        attribute.components_count,
        attribute.type,
        attribute.normalized,
        accumulated_stride,
        attribute.offset
    );
    glVertexAttribPointer(
        attribute.index,
        attribute.components_count,
        attribute.type,
        attribute.normalized,
        accumulated_stride,
        reinterpret_cast<const void*>(attribute.offset)
    );
}

void enable_vertex_attrib_array(const vertex_array_attribute& attrib) {
    LOG_DEBUG("glEnableVertexAttribArray: {}", attrib.index);
    glEnableVertexAttribArray(attrib.index);
}

} // namespace

vertex_array::vertex_array()
    : finalizer{ [](vertex_array& self) {
          // TODO(@usatiynyan): more than one VAO?
          glDeleteVertexArrays(1, &self.internal_);
          LOG_DEBUG("glDeleteVertexArrays: {}", self.internal_);
      } },
      internal_{ [] {
          GLuint va_object = 0;
          // TODO(@usatiynyan): more than one VAO?
          glGenVertexArrays(1, &va_object);
          LOG_DEBUG("glGenVertexArrays: {}", va_object);
          return va_object;
      }() } {}

bound_vertex_array vertex_array::bind() const { return bound_vertex_array{ *this }; }

bound_vertex_array::bound_vertex_array(const vertex_array& va)
    : finalizer{ [](bound_vertex_array&) {
          LOG_DEBUG("glBindVertexArray: {}", 0);
          glBindVertexArray(0);
      } } {
    LOG_DEBUG("glBindVertexArray: {}", va.internal());
    glBindVertexArray(va.internal());
}

void vertex_array_builder::attribute(vertex_array_attribute va_attribute) {
    accumulated_stride_ += va_attribute.stride;
    attributes_.push_back(va_attribute);
}

vertex_array vertex_array_builder::submit() && {
    ASSERT(!attributes_.empty());
    for (const vertex_array_attribute& attribute : attributes_) {
        vertex_attrib_pointer(attribute, accumulated_stride_);
        enable_vertex_attrib_array(attribute);
    }
    bound_.reset();
    return std::move(va_).value();
}

} // namespace sl::gfx
