//
// Created by usatiynyan on 12/2/23.
//

#pragma once

#include <filesystem>
#include <memory>
#include <tl/optional.hpp>

namespace stb {
struct Image {
    using Deleter = void (*)(uint8_t *);

    std::unique_ptr<uint8_t, Deleter> data;
    std::array<int, 2> dimensions;
    int channels;
};

tl::optional<Image> image_load(const std::filesystem::path& path, int desired_channels = 0, bool flip_vertically = true);
} // namespace stb
