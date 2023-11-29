//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include <spdlog/spdlog.h>

#ifdef NDEBUG
#define LOG_DEBUG(...) (static_cast<void>(0))
#else // NDEBUG
#define LOG_DEBUG sl::log::debug
#endif // NDEBUG

namespace sl {
namespace log = spdlog;
} // namespace sl
