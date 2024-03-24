//
// Created by usatiynyan on 12/2/23.
//

#include "stb/image.hpp"

#include <libassert/assert.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x) ASSERT(x)

#include "stb_image.h"

#undef STBI_ASSERT
#undef STB_IMAGE_IMPLEMENTATION

namespace stb {
tl::optional<Image> image_load(const std::filesystem::path& path, int desired_channels, bool flip_vertically) {
    stbi_set_flip_vertically_on_load(flip_vertically);

    int width = 0;
    int height = 0;
    int channels = 0;
    std::unique_ptr<uint8_t, Image::Deleter> data{
        stbi_load(path.c_str(), &width, &height, &channels, desired_channels),
        [](uint8_t* image_data) { stbi_image_free(static_cast<void*>(image_data)); },
    };
    if (!data) {
        return tl::nullopt;
    }
    return Image{ std::move(data), width, height, channels };
}
} // namespace stb
