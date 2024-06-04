//
// Created by usatiynyan.
//

#include "sl/gfx/common/log.hpp"

namespace sl::gfx {

spdlog::logger& logger() {
    static spdlog::logger logger{ "sl::gfx" };
    return logger;
}

} // namespace sl::gfx
