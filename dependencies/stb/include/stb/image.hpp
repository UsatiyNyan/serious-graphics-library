//
// Created by usatiynyan.
//

#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <span>

#include <tl/optional.hpp>

namespace stb {

struct Image {
    using deleter_type = void (*)(uint8_t*);

    std::unique_ptr<uint8_t, deleter_type> data;
    std::array<int, 2> dimensions;
    int channels;
};

tl::optional<Image>
    image_load(const std::filesystem::path& path, int desired_channels = 0, bool flip_vertically = true);

tl::optional<Image>
    image_load_from_memory(std::span<const std::uint8_t> buffer, int desired_channels = 0, bool flip_vertically = true);

} // namespace stb
