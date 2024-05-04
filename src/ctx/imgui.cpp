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

imgui_frame imgui_context::new_frame() const { return imgui_frame{ *this }; }

imgui_frame::imgui_frame(const imgui_context&)
    : finalizer{ [](imgui_frame&) {
          ImGui::Render();
          ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      } } {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

imgui_window imgui_frame::begin(const char* name) { return imgui_window{ *this, name }; }

imgui_window::imgui_window(imgui_frame&, const char* name)
    : finalizer{ [](imgui_window&) { ImGui::End(); } }, is_begun_{ ImGui::Begin(name) } {}

} // namespace sl::gfx
