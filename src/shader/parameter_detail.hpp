//
// Created by usatiynyan on 11/22/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"

namespace sl::gfx::detail {
template <typename ParameterRetriever>
GLint get_parameter(ParameterRetriever parameter_retriever, GLuint object, GLenum parameter_name) {
    GLint result = 0;
    parameter_retriever(object, parameter_name, &result);
    return result;
}

template <typename ParameterRetriever>
GLint get_parameter(ParameterRetriever parameter_retriever, GLenum parameter_name) {
    GLint result = 0;
    parameter_retriever(parameter_name, &result);
    return result;
}
} // namespace sl::gfx::detail
