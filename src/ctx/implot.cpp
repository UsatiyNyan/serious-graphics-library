//
// Created by usatiynyan on 1/4/24.
//

#include "sl/gfx/ctx/implot.hpp"

namespace sl::gfx {

implot_context::implot_context(const imgui_context&) : finalizer{ [](implot_context&) { ImPlot::DestroyContext(); } } {
    ImPlot::CreateContext();
}

} // namespace sl::gfx
