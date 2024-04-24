//
// Created by usatiynyan on 1/4/24.
//

#include "sl/gfx/ctx/implot.hpp"

namespace sl::gfx {

implot_context::implot_context(const imgui_context&) {
    ImPlot::CreateContext();
}

implot_context::~implot_context() {
    ImPlot::DestroyContext();
}

} // namespace sl::gfx
