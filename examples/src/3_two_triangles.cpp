//
// Created by usatiynyan on 11/28/23.
//

#include "sl/gfx.hpp"

#include <assert.hpp>
#include <spdlog/spdlog.h>

using namespace sl::gfx;

void debug_unbind() {
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename DataType>
void draw(
    const ShaderProgram& sp,
    const VertexArray& va,
    const Buffer<DataType, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>& eb
) {
    const auto sp_bind = sp.bind();
    const auto va_bind = va.bind();
    glDrawElements(GL_TRIANGLES, eb.data_size(), type_map<DataType>::value, nullptr);
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

    constexpr auto create_triangle_shader = [](Color4F color4f) {
        const auto fragment_shader_source = fmt::format(
            "#version 460 core\n"
            "out vec4 FragColor;\n"
            "void main() {{\n"
            "    FragColor = vec4({});"
            "\n}}",
            fmt::join(std::array{ color4f.red, color4f.green, color4f.blue, color4f.alpha }, ", ")
        );
        std::array<const Shader, 2> shaders{
            *ASSERT(Shader::load_from_file(ShaderType::VERTEX, "shaders/3_two_triangles.vert")),
            *ASSERT(Shader::load_from_source(ShaderType::FRAGMENT, fragment_shader_source)),
        };
        return ShaderProgram{ std::span{ shaders } };
    };

    using buffers_type = std::tuple<
        Buffer<float, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
        Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
        VertexArray>;

    constexpr auto create_triangle_buffers = [](std::span<const float, 3 * 3> vertices) -> buffers_type {
        VertexArrayBuilder va_builder;
        va_builder.attribute<3, float>();
        auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vertices);
        constexpr std::array<unsigned, 3> indices{ 0u, 1u, 2u };
        auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
        auto va = std::move(va_builder).submit();
        return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
    };

    constexpr std::array left_triangle_vertices{
        -0.0f, -0.5f, 0.0f, // bottom right
        -1.0f, -0.5f, 0.0f, // bottom left
        -1.0f, +0.5f, 0.0f, // top left
    };
    constexpr std::array right_triangle_vertices{
        +1.0f, -0.5f, 0.0f, // bottom right
        +0.0f, -0.5f, 0.0f, // bottom left
        +0.0f, +0.5f, 0.0f, // top left
    };
    const std::array triangles{
        std::pair{
            create_triangle_shader(Color4F{ 1.0f, 0.5f, 0.2f, 1.0f }),
            create_triangle_buffers(std::span{ left_triangle_vertices }),
        },
        std::pair{
            create_triangle_shader(Color4F{ 0.7f, 0.1f, 0.7f, 1.0f }),
            create_triangle_buffers(std::span{ right_triangle_vertices }),
        },
    };

    // VVV debug
    debug_unbind();
    // ^^^ debug

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        for (const std::pair<ShaderProgram, buffers_type>& triangle : triangles) {
            const auto& [sp, buffers] = triangle;
            const auto& [vb, eb, va] = buffers;
            draw(sp, va, eb);
        }

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
