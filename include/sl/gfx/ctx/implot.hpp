//
// Created by usatiynyan on 1/4/24.
//

#pragma once

#include "sl/gfx/ctx/imgui.hpp"

#include "implot.h"

namespace sl::gfx {

class implot_context {
public:
    explicit implot_context(const imgui_context&);
    ~implot_context();
};

} // namespace sl::gfx
