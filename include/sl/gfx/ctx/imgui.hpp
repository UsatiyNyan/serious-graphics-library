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
#undef IMGUI_IMPL_OPENGL_LOADER_CUSTOM

#include <sl/meta/lifetime/finalizer.hpp>

#include <fmt/format.h>

namespace sl::gfx {

class imgui_frame;

class imgui_context : public meta::finalizer<imgui_context> {
public:
    enum class Theme {
        DARK,
        LIGHT,
        CLASSIC,
    };

    imgui_context(const context::options& options, const window& window, Theme theme = Theme::DARK);

    imgui_frame new_frame() const;
};

class imgui_window;

class imgui_frame : public meta::finalizer<imgui_frame> {
public:
    explicit imgui_frame(const imgui_context&);

    imgui_window begin(const char* name);
};

class imgui_window : public meta::finalizer<imgui_window> {
public:
    explicit imgui_window(imgui_frame&, const char* name);

    [[nodiscard]] operator bool() const { return is_begun_; }

private:
    bool is_begun_;
};

} // namespace sl::gfx
