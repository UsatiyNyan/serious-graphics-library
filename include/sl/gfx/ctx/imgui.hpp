//
// Created by usatiynyan on 1/1/24.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/ctx/window.hpp"

// clang-format off
#include "imgui.h"
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
// clang-format on

#include "fmt/format.h"

namespace sl::gfx {

class ImGuiContext {
public:
    enum class Theme {
        DARK,
        LIGHT,
        CLASSIC,
    };

    ImGuiContext(const Context::Options& ctx_options, const Window& window, Theme theme = Theme::DARK);
    ~ImGuiContext();

    void new_frame();
    void render();
};

} // namespace sl::gfx
