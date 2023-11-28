//
// Created by usatiynyan on 11/26/23.
//

#pragma once


#include "sl/gfx/common/finalizer.hpp"
#include "sl/gfx/common/log.hpp"
#include "sl/gfx/common/vendors.hpp"

#include <span>

namespace sl::gfx {
enum class BufferType : GLenum {
    ARRAY = GL_ARRAY_BUFFER, //  attributes
    ATOMIC_COUNTER = GL_ATOMIC_COUNTER_BUFFER, // Atomic counter storage
    COPY_READ = GL_COPY_READ_BUFFER, // Buffer copy source
    COPY_WRITE = GL_COPY_WRITE_BUFFER, // Buffer copy destination
    DISPATCH_INDIRECT = GL_DISPATCH_INDIRECT_BUFFER, // Indirect compute dispatch commands
    DRAW_INDIRECT = GL_DRAW_INDIRECT_BUFFER, // Indirect command arguments
    ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER, //  array indices
    PIXEL_PACK = GL_PIXEL_PACK_BUFFER, // Pixel read target
    PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER, // Texture data source
    QUERY = GL_QUERY_BUFFER, // Query result buffer
    SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER, // Read-write storage for shaders
    TEXTURE = GL_TEXTURE_BUFFER, // Texture data buffer
    TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER, // Transform feedback buffer
    UNIFORM = GL_UNIFORM_BUFFER, // Uniform block storage
};

// STREAM - The data store contents will be modified once and used at most a few times.
// STATIC - The data store contents will be modified once and used many times.
// DYNAMIC - The data store contents will be modified repeatedly and used many times.
// DRAW - The data store contents are modified by the application,
//   and used as the source for GL drawing and image specification commands.
// READ - The data store contents are modified by reading data from the GL,
//   and used to return that data when queried by the application.
// COPY - The data store contents are modified by reading data from the GL,
//   and used as the source for GL drawing and image specification commands.
enum class BufferUsage : GLenum {
    STREAM_DRAW = GL_STREAM_DRAW,
    STREAM_READ = GL_STREAM_READ,
    STREAM_COPY = GL_STREAM_COPY,
    STATIC_DRAW = GL_STATIC_DRAW,
    STATIC_READ = GL_STATIC_READ,
    STATIC_COPY = GL_STATIC_COPY,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    DYNAMIC_READ = GL_DYNAMIC_READ,
    DYNAMIC_COPY = GL_DYNAMIC_COPY,
};

// TODO(@usatiynyan): solve unbind?
template <typename DataType, BufferType type_, BufferUsage usage_>
class Buffer : public finalizer<Buffer<DataType, type_, usage_>> {
    class Bind {
    public:
        explicit Bind(const Buffer& buffer) {
            log::debug("glBindBuffer: {}", *buffer);
            glBindBuffer(static_cast<GLenum>(type_), *buffer);
        }

        template <std::size_t extent>
        void set_data(std::span<DataType, extent> data) {
            log::debug("glBufferData: size={}", data.size());
            glBufferData(
                static_cast<GLenum>(type_), sizeof(DataType) * data.size(), data.data(), static_cast<GLenum>(usage_)
            );
        }
    };

public:
    Buffer()
        : finalizer<Buffer>{ [](Buffer& self) {
              // TODO(@usatiynyan): more than one buffer?
              glDeleteBuffers(1, &self.object_);
              log::debug("glDeleteBuffers: {}", self.object_);
          } },
          object_{ [] {
              GLuint buffer = 0;
              glGenBuffers(1, &buffer);
              log::debug("glGenBuffers: {}", buffer);
              return buffer;
          }() } {}

    [[nodiscard]] GLuint operator*() const { return object_; }
    [[nodiscard]] auto bind() { return Bind{ *this }; }
    [[nodiscard]] auto bind() const { return Bind{ *this }; }

private:
    GLuint object_;
};
} // namespace sl::gfx
