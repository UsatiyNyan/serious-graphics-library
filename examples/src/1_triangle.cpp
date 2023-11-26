//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx.hpp"

#include <assert.hpp>
#include <spdlog/spdlog.h>

using namespace sl::gfx;

void draw(const ShaderProgram& shader_program, const VertexArray& va) {
    shader_program.use();
    const auto va_bound = va.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto ctx = *ASSERT(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    const Size2I window_size{ 800, 600 };
    const auto window = ASSERT(Window::create(ctx, "blank", window_size));
    window->FramebufferSize_cb = [&window](GLsizei width, GLsizei height) {
        Window::Current{ *window }.viewport(Vec2I{}, Size2I{ width, height });
    };
    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    const std::array shaders{
        *ASSERT(Shader::load_from_file(ShaderType::VERTEX, "shaders/1_triangle.vert")),
        *ASSERT(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/1_triangle.frag")),
    };
    ShaderProgram shader_program{ std::span{ shaders } };

    VertexArrayBuilder va_builder;
    va_builder.attribute<3, float>();
    constexpr std::array<float, 3 * 3> vertices{
        -0.5f, -0.5f, 0.0f, //
        0.5f,  -0.5f, 0.0f, //
        0.0f,  0.5f,  0.0f, //
    };
    const auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(std::span{ vertices });
    const auto va = std::move(va_builder).submit();

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        draw(shader_program, va);

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
