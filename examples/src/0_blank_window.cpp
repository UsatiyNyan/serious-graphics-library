//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx.hpp"

#include <assert.hpp>
#include <spdlog/spdlog.h>

using namespace sl::gfx;

int main() {
    spdlog::set_level(spdlog::level::debug);

    const auto ctx = ASSERT(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));

    const Size2I window_size{ 800, 600 };
    const auto window = ASSERT(Window::create(*ctx, "blank", window_size));

    window->FramebufferSize_cb = [&window](GLsizei width, GLsizei height) {
        Window::Current{ *window }.viewport(Vec2I{}, Size2I{ width, height });
    };

    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        current_window.clear(GL_COLOR_BUFFER_BIT);

        current_window.swap_buffers();
        ctx->poll_events();
    }

    return 0;
}
