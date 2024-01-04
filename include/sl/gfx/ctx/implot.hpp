//
// Created by usatiynyan on 1/4/24.
//

#pragma once

#include "sl/gfx/ctx/imgui.hpp"

#include "implot.h"

namespace sl::gfx {

class ImPlotContext {
public:
    explicit ImPlotContext(const ImGuiContext&);
    ~ImPlotContext();
};

} // namespace sl::gfx
