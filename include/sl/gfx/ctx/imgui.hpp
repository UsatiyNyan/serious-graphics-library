//
// Created by usatiynyan on 1/1/24.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/ctx/window.hpp"

#include <imgui.h>
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <sl/meta/lifetime/finalizer.hpp>

#include <fmt/format.h>

namespace sl::gfx {

class imgui_context : public meta::finalizer<imgui_context> {
public:
    enum class Theme {
        DARK,
        LIGHT,
        CLASSIC,
    };

    imgui_context(const context::options& options, const window& window, Theme theme = Theme::DARK);

    void new_frame();
    void render();
};

} // namespace sl::gfx
