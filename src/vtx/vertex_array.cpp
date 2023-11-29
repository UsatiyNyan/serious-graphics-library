//
// Created by usatiynyan on 11/26/23.
//

#include "sl/gfx/vtx/vertex_array.hpp"

#include "sl/gfx/common/log.hpp"

#include <assert.hpp>

namespace sl::gfx {
void VertexArrayAttribute::set_pointer(GLsizei accumulated_stride) {
    LOG_DEBUG("glVertexAttribPointer: {}", index_);
    glVertexAttribPointer(
        index_, components_size_, type_, normalized_, accumulated_stride, reinterpret_cast<const void*>(offset_)
    );
}

void VertexArrayAttribute::enable() {
    LOG_DEBUG("glEnableVertexAttribArray: {}", index_);
    glEnableVertexAttribArray(index_);
}

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
    ASSERT(!attributes_.empty());
    for (VertexArrayAttribute& attribute : attributes_) {
        attribute.set_pointer(accumulated_stride_);
        attribute.enable();
    }
    bind_.reset();
    return std::move(va_).value();
}
} // namespace sl::gfx
