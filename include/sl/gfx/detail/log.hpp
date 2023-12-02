//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include <string>

#include "sl/gfx/common/vendors.hpp"

namespace sl::gfx::detail {
template <typename LogRetriever>
std::string get_log(LogRetriever log_retriever, GLuint object) {
    constexpr size_t MAX_LOG_LENGTH = 1024;
    std::string log_text(MAX_LOG_LENGTH, 0);
    GLsizei log_length = 0;
    log_retriever(object, MAX_LOG_LENGTH, &log_length, log_text.data());
    log_text.resize(log_length);
    return log_text;
}
}  // namespace SL::graphics::log

