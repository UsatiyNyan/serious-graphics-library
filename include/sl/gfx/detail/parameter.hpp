//
// Created by usatiynyan on 11/22/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

namespace sl::gfx::detail {

template <typename ParameterRetriever, typename... Args>
GLint get_parameter(ParameterRetriever parameter_retriever, Args... args) {
    GLint result = 0;
    parameter_retriever(args..., &result);
    return result;
}

} // namespace sl::gfx::detail
