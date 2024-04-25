//
// Created by usatiynyan on 1/1/24.
//

#include "sl/gfx/ctx/imgui.hpp"

namespace sl::gfx {

imgui_context::imgui_context(const context::options& options, const window& window, Theme theme)
    : finalizer{ [](imgui_context&) {
          ImGui_ImplOpenGL3_Shutdown();
          ImGui_ImplGlfw_Shutdown();
          ImGui::DestroyContext();
      } } {

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window.internal(), true);
    const auto glsl_version = fmt::format("#version {}{}0", options.version_major, options.version_minor);
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

void imgui_context::new_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_context::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace sl::gfx
