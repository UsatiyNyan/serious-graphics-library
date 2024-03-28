//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include <cmath>
#include <cstdint>

namespace sl::gfx {
namespace detail {

// TODO: std::array or std::span view
template <std::size_t N, typename T>
struct Vec;

template <typename T>
struct Vec<2, T> {
    T x;
    T y;
};

} // namespace detail

using Vec2I = detail::Vec<2, std::int32_t>;
using Vec2F = detail::Vec<2, std::float_t>;
using Vec2D = detail::Vec<2, std::double_t>;

} // namespace sl::gfx
