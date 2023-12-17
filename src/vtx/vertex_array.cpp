//
// Created by usatiynyan on 11/26/23.
//

#include "sl/gfx/vtx/vertex_array.hpp"

#include "sl/gfx/common/log.hpp"

#include <assert.hpp>

namespace sl::gfx {
namespace {

void vertex_attrib_pointer(const VertexArrayAttribute& attrib, GLsizei accumulated_stride) {
    LOG_DEBUG("glVertexAttribPointer: {}", attrib.index);
    glVertexAttribPointer(
        attrib.index,
        attrib.components_count,
        attrib.type,
        attrib.normalized,
        accumulated_stride,
        reinterpret_cast<const void*>(attrib.offset)
    );
}

void enable_vertex_attrib_array(const VertexArrayAttribute& attrib) {
    LOG_DEBUG("glEnableVertexAttribArray: {}", attrib.index);
    glEnableVertexAttribArray(attrib.index);
}

} // namespace

VertexArray::Bind::Bind(const VertexArray& va)
    : finalizer{ [](Bind& self) {
          LOG_DEBUG("glBindVertexArray: {}", 0);
          glBindVertexArray(0);
      } } {
    LOG_DEBUG("glBindVertexArray: {}", *va);
    glBindVertexArray(*va);
}

VertexArray::VertexArray()
    : finalizer{ [](VertexArray& self) {
          // TODO(@usatiynyan): more than one VAO?
          glDeleteVertexArrays(1, &self.object_);
          LOG_DEBUG("glDeleteVertexArrays: {}", self.object_);
      } },
      object_{ [] {
          GLuint va_object = 0;
          // TODO(@usatiynyan): more than one VAO?
          glGenVertexArrays(1, &va_object);
          LOG_DEBUG("glGenVertexArrays: {}", va_object);
          return va_object;
      }() } {}

VertexArray VertexArrayBuilder::submit() && {
    ASSERT(!attribs_.empty());
    for (const VertexArrayAttribute& attrib : attribs_) {
        vertex_attrib_pointer(attrib, accumulated_stride_);
        enable_vertex_attrib_array(attrib);
    }
    bind_.reset();
    return std::move(va_).value();
}
} // namespace sl::gfx
