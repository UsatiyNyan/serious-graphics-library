//
// Created by usatiynyan on 11/28/23.
//

#include "sl/gfx.hpp"

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>

using namespace sl::gfx;

void debug_unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename DataType>
void draw(
    const ShaderProgram& sp,
    const VertexArray& va,
    const Buffer<DataType, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>& eb
) {
    [[maybe_unused]] const auto sp_bind = sp.bind();
    const auto va_bind = va.bind();
    glDrawElements(GL_TRIANGLES, eb.data_size(), type_map<DataType>::value, nullptr);
}

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto ctx = *ASSERT_VAL(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    const Size2I window_size{ 800, 600 };
    const auto window = ASSERT_VAL(Window::create(ctx, "02_rectangle", window_size));
    (void)window->FramebufferSize_cb.connect([&window](GLsizei width, GLsizei height) {
        Window::Current{ *window }.viewport(Vec2I{}, Size2I{ width, height });
    });
    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    const std::array shaders{
        *ASSERT_VAL(Shader::load_from_file(ShaderType::VERTEX, "shaders/02_rectangle.vert")),
        *ASSERT_VAL(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/02_rectangle.frag")),
    };
    const auto sp = *ASSERT_VAL(ShaderProgram::build(std::span{ shaders }));

    VertexArrayBuilder va_builder;
    va_builder.attribute<3, float>();
    constexpr std::array<float, 3 * 4> vertices{
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f // top left
    };
    const auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(std::span{ vertices });
    constexpr std::array<unsigned, 3 * 2> indices{
        0u, 1u, 3u, // first triangle
        1u, 2u, 3u // second triangle
    };
    const auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
    const auto va = std::move(va_builder).submit();

    // VVV debug
    debug_unbind();
    // ^^^ debug

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        draw(sp, va, eb);

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
