//
// Created by usatiynyan.
//

#include "stb/image.hpp"

#include <bit>
#include <fstream>
#include <libassert/assert.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) ASSERT(x)

#include "stb_image.h"

#undef STBI_ASSERT
#undef STB_IMAGE_IMPLEMENTATION

namespace stb {

tl::optional<Image> image_load(const std::filesystem::path& path, int desired_channels, bool flip_vertically) {
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file) {
        return tl::nullopt;
    }

    const std::streamsize size = file.tellg();
    if (size < 0) {
        return tl::nullopt;
    }
    file.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> buffer(size);
    if (!file.read(std::bit_cast<char*>(buffer.data()), size)) {
        return tl::nullopt;
    }

    return image_load_from_memory(std::span(std::as_const(buffer)), desired_channels, flip_vertically);
}

tl::optional<Image>
    image_load_from_memory(std::span<const std::uint8_t> buffer, int desired_channels, bool flip_vertically) {
    stbi_set_flip_vertically_on_load(flip_vertically);

    int width = 0;
    int height = 0;
    int channels = 0;
    std::unique_ptr<uint8_t, Image::deleter_type> data{
        stbi_load_from_memory(buffer.data(), buffer.size_bytes(), &width, &height, &channels, desired_channels),
        [](uint8_t* image_data) { stbi_image_free(static_cast<void*>(image_data)); },
    };
    if (!data) {
        return tl::nullopt;
    }
    return Image{ std::move(data), { width, height }, channels };
}

} // namespace stb
