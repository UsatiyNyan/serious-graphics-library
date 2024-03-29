//
// Created by usatiynyan on 1/1/24.
//

#include "sl/gfx.hpp"

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>

int main() {
    spdlog::set_level(spdlog::level::debug);

    const sl::gfx::Context::Options ctx_options{ 4, 6, GLFW_OPENGL_CORE_PROFILE };
    auto ctx = *ASSERT_VAL(sl::gfx::Context::create(ctx_options));
    const sl::gfx::Size2I window_size{ 800, 600 };
    const auto window = ASSERT_VAL(sl::gfx::Window::create(ctx, "08_imgui", window_size));
    (void)window->FramebufferSize_cb.connect([&window](GLsizei width, GLsizei height) {
        sl::gfx::Window::Current{ *window }.viewport(sl::gfx::Vec2I{}, sl::gfx::Size2I{ width, height });
    });
    auto current_window =
        window->make_current(sl::gfx::Vec2I{}, window_size, sl::gfx::Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    sl::gfx::ImGuiContext imgui_context{ ctx_options, *window };

    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO(); // Get the ImGui IO instance

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);
        imgui_context.new_frame();

        ImGui::ShowDemoWindow();

        imgui_context.render();
        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
