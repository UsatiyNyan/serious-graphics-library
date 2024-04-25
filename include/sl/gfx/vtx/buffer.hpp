//
// Created by usatiynyan on 11/26/23.
//

#pragma once

#include "sl/gfx/common/log.hpp"
#include "sl/gfx/common/vendors.hpp"

#include "sl/meta/lifetime/finalizer.hpp"

#include <span>
#include <tl/optional.hpp>

namespace sl::gfx {
enum class buffer_type : GLenum {
    array = GL_ARRAY_BUFFER, // Vertex attributes
    atomic_counter = GL_ATOMIC_COUNTER_BUFFER, // Atomic counter storage
    copy_read = GL_COPY_READ_BUFFER, // Buffer copy source
    copy_write = GL_COPY_WRITE_BUFFER, // Buffer copy destination
    dispatch_indirect = GL_DISPATCH_INDIRECT_BUFFER, // Indirect compute dispatch commands
    draw_indirect = GL_DRAW_INDIRECT_BUFFER, // Indirect command arguments
    element_array = GL_ELEMENT_ARRAY_BUFFER, //  array indices
    pixel_pack = GL_PIXEL_PACK_BUFFER, // Pixel read target
    pixel_unpack = GL_PIXEL_UNPACK_BUFFER, // Texture data source
    query = GL_QUERY_BUFFER, // Query result buffer
    shader_storage = GL_SHADER_STORAGE_BUFFER, // Read-write storage for shaders
    texture = GL_TEXTURE_BUFFER, // Texture data buffer
    transform_feedback = GL_TRANSFORM_FEEDBACK_BUFFER, // Transform feedback buffer
    uniform = GL_UNIFORM_BUFFER, // Uniform block storage
};

// stream - The data store contents will be modified once and used at most a few times.
// static - The data store contents will be modified once and used many times.
// dynamic - The data store contents will be modified repeatedly and used many times.
// draw - The data store contents are modified by the application,
//   and used as the source for GL drawing and image specification commands.
// read - The data store contents are modified by reading data from the GL,
//   and used to return that data when queried by the application.
// copy - The data store contents are modified by reading data from the GL,
//   and used as the source for GL drawing and image specification commands.
enum class buffer_usage : GLenum {
    stream_draw = GL_STREAM_DRAW,
    stream_read = GL_STREAM_READ,
    stream_copy = GL_STREAM_COPY,
    static_draw = GL_STATIC_DRAW,
    static_read = GL_STATIC_READ,
    static_copy = GL_STATIC_COPY,
    dynamic_draw = GL_DYNAMIC_DRAW,
    dynamic_read = GL_DYNAMIC_READ,
    dynamic_copy = GL_DYNAMIC_COPY,
};

enum class buffer_access : GLenum {
    read_only = GL_READ_ONLY,
    write_only = GL_WRITE_ONLY,
    read_write = GL_READ_WRITE,
};

template <typename T, buffer_type type, buffer_usage usage>
class bound_buffer;

template <typename T, buffer_type type, buffer_access access, std::size_t size>
class mapped_buffer;

template <typename T, buffer_type type, buffer_usage usage>
class buffer : public meta::finalizer<buffer<T, type, usage>> {
    friend class bound_buffer<T, type, usage>; // data_size_

public:
    buffer()
        : meta::finalizer<buffer>{ [](buffer& self) {
              glDeleteBuffers(1, &self.object_);
              LOG_DEBUG("glDeleteBuffers: {}", self.object_);
          } },
          internal_{ [] {
              GLuint buffer = 0;
              // TODO(@usatiynyan): more than one buffer?
              glGenBuffers(1, &buffer);
              LOG_DEBUG("glGenBuffers: {}", buffer);
              return buffer;
          }() } {}

    [[nodiscard]] bound_buffer<T, type, usage> bind();
    [[nodiscard]] const bound_buffer<T, type, usage> bind() const;

    [[nodiscard]] std::size_t data_size() const { return data_size_; }
    [[nodiscard]] GLuint internal() const { return internal_; }

private:
    GLuint internal_;
    std::size_t data_size_ = 0;
};

template <typename T, buffer_type type, buffer_usage usage>
class bound_buffer {
public:
    explicit bound_buffer(buffer<T, type, usage>& buffer) : buffer_{ buffer } {
        LOG_DEBUG("glBindBuffer: {}", *buffer_);
        glBindBuffer(static_cast<GLenum>(type), *buffer_);
    }

    template <std::size_t size>
    void initialize_data() {
        LOG_DEBUG("glBufferData: size={}, data=nullptr", size);
        glBufferData(static_cast<GLenum>(type), sizeof(T) * size, nullptr, static_cast<GLenum>(usage));
        buffer_.data_size_ = size;
    }
    template <std::size_t extent>
    void set_data(std::span<const T, extent> data) {
        LOG_DEBUG("glBufferData: size={}", data.size());
        glBufferData(static_cast<GLenum>(type), sizeof(T) * data.size(), data.data(), static_cast<GLenum>(usage));
        buffer_.data_size_ = data.size();
    }

    template <GLuint index>
        requires(type == buffer_type::shader_storage)
    void bind_base() {
        LOG_DEBUG("glBindBufferBase: {} index={}", *buffer_, index);
        glBindBufferBase(static_cast<GLenum>(type), index, *buffer_);
    }

    template <buffer_access access, std::size_t size>
    [[nodiscard]] tl::optional<mapped_buffer<T, type, access, size>> map();
    template <std::size_t size>
    [[nodiscard]] tl::optional<mapped_buffer<T, type, buffer_access::read_only, size>> map() const;

private:
    buffer<T, type, usage>& buffer_;
};

template <typename T, buffer_type type, buffer_access access, std::size_t size>
class mapped_buffer : public meta::finalizer<mapped_buffer<T, type, access, size>> {
    mapped_buffer(T* data)
        : meta::finalizer<mapped_buffer<T, type, access, size>>{ [](mapped_buffer&) {
              glUnmapBuffer(static_cast<GLenum>(type));
          } },
          data_{ data, size } {}

public:
    static tl::optional<mapped_buffer> create() {
        void* data = glMapBuffer(static_cast<GLenum>(type), static_cast<GLenum>(access));
        if (data == nullptr) {
            return tl::nullopt;
        }
        return mapped_buffer(static_cast<T*>(data));
    }

    [[nodiscard]] auto data() { return data_; }
    [[nodiscard]] auto data() const { return data_; }

private:
    std::span<T, size> data_;
};

template <typename T, buffer_type type, buffer_usage usage>
bound_buffer<T, type, usage> buffer<T, type, usage>::bind() {
    return bound_buffer<T, type, usage>{ *this };
}

template <typename T, buffer_type type, buffer_usage usage>
const bound_buffer<T, type, usage> buffer<T, type, usage>::bind() const {
    return bound_buffer<T, type, usage>{ *this };
}

template <typename T, buffer_type type, buffer_usage usage>
template <buffer_access access, std::size_t size>
tl::optional<mapped_buffer<T, type, access, size>> bound_buffer<T, type, usage>::map() {
    return mapped_buffer<T, type, access, size>::create();
}

template <typename T, buffer_type type, buffer_usage usage>
template <std::size_t size>
tl::optional<mapped_buffer<T, type, buffer_access::read_only, size>> bound_buffer<T, type, usage>::map() const {
    return mapped_buffer<T, type, buffer_access::read_only, size>::create();
}

} // namespace sl::gfx
