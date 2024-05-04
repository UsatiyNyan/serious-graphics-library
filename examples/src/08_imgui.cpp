//
// Created by usatiynyan on 1/1/24.
//

#include "sl/gfx.hpp"

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>

namespace gfx = sl::gfx;

int main() {
    spdlog::set_level(spdlog::level::debug);

    constexpr gfx::context::options ctx_options{ 4, 6, GLFW_OPENGL_CORE_PROFILE };
    auto ctx = ASSERT_VAL(gfx::context::create(ctx_options));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "08_imgui", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    gfx::imgui_context imgui_context{ ctx_options, *window };

    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO(); // Get the ImGui IO instance

    while (!current_window.should_close()) {
        ctx->poll_events();

        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        current_window.clear(GL_COLOR_BUFFER_BIT);

        {
            auto imgui_frame = imgui_context.new_frame();
            ImGui::ShowDemoWindow();
        }

        current_window.swap_buffers();
    }

    return 0;
}
