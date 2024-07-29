//
// Created by usatiynyan on 11/22/23.
//

#pragma once

#include "sl/gfx/detail/vendors.hpp"

#include <string>

namespace sl::gfx::detail {

template <typename LogRetriever>
std::string get_log(LogRetriever log_retriever, GLuint object) {
    constexpr size_t MAX_LOG_LENGTH = 1024;
    std::string log_text(MAX_LOG_LENGTH, 0);
    GLsizei log_length = 0;
    log_retriever(object, MAX_LOG_LENGTH, &log_length, log_text.data());
    log_text.resize(static_cast<std::size_t>(log_length));
    return log_text;
}


template <typename ParameterRetriever, typename... Args>
GLint get_parameter(ParameterRetriever parameter_retriever, Args... args) {
    GLint result = 0;
    parameter_retriever(args..., &result);
    return result;
}

} // namespace sl::gfx::detail
