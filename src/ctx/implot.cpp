//
// Created by usatiynyan on 1/4/24.
//

#include "sl/gfx/ctx/implot.hpp"

namespace sl::gfx {

ImPlotContext::ImPlotContext(const ImGuiContext&) {
    ImPlot::CreateContext();
}

ImPlotContext::~ImPlotContext() {
    ImPlot::DestroyContext();
}

} // namespace sl::gfx
