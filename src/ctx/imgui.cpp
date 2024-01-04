//
// Created by usatiynyan on 1/1/24.
//

#include "sl/gfx/ctx/imgui.hpp"

namespace sl::gfx {

ImGuiContext::ImGuiContext(const Context::Options& ctx_options, const Window& window, Theme theme) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.glfw_window(), true);
    const auto glsl_version = fmt::format("#version {}{}0", ctx_options.version_major, ctx_options.version_minor);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());

    // Setup ImGui style
    switch (theme) {
    case Theme::DARK:
        ImGui::StyleColorsDark();
        break;
    case Theme::LIGHT:
        ImGui::StyleColorsLight();
        break;
    case Theme::CLASSIC:
        ImGui::StyleColorsClassic();
        break;
    }
}

ImGuiContext::~ImGuiContext() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiContext::new_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiContext::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace sl::gfx
