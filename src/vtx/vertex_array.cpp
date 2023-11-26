//
// Created by usatiynyan on 11/26/23.
//

#include "sl/gfx/vtx/vertex_array.hpp"

#include "sl/gfx/common/log.hpp"

#include <assert.hpp>

namespace sl::gfx {
void VertexArrayAttribute::set_pointer(GLsizei accumulated_stride) {
    log::debug("glVertexAttribPointer: {}", index_);
    glVertexAttribPointer(
        index_, components_size_, type_, normalized_, accumulated_stride, reinterpret_cast<const void*>(offset_)
    );
}

void VertexArrayAttribute::enable() {
    log::debug("glEnableVertexAttribArray: {}", index_);
    glEnableVertexAttribArray(index_);
}

VertexArray::VertexArray()
    : finalizer{ [](VertexArray& self) {
          // TODO(@usatiynyan): more than one VAO?
          glDeleteVertexArrays(1, &self.object_);
          log::debug("glDeleteVertexArrays: {}", self.object_);
      } },
      object_{ [] {
          GLuint va_object = 0;
          // TODO(@usatiynyan): more than one VAO?
          glGenVertexArrays(1, &va_object);
          log::debug("glGenVertexArrays: {}", va_object);
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
