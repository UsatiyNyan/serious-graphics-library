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

class Draw {
public:
    Draw(const ShaderProgram& sp, const VertexArray& va) : sp_bind_{ sp }, va_bind_{ va } {}

    [[nodiscard]] const auto& get_sp_bind() const { return sp_bind_; }

    template <typename DataType>
    void elements(const Buffer<DataType, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>& eb) {
        glDrawElements(GL_TRIANGLES, eb.data_size(), type_map<DataType>::value, nullptr);
    }

private:
    ShaderProgram::Bind sp_bind_;
    VertexArray::Bind va_bind_;
};

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto ctx = *ASSERT(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    const Size2I window_size{ 800, 600 };
    const auto window = ASSERT(Window::create(ctx, "blank", window_size));
    window->FramebufferSize_cb = [&window](GLsizei width, GLsizei height) {
        Window::Current{ *window }.viewport(Vec2I{}, Size2I{ width, height });
    };
    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    constexpr auto create_triangle_shader = [] {
        std::array<const Shader, 2> shaders{
            *ASSERT(Shader::load_from_file(ShaderType::VERTEX, "shaders/4_colored_triangle.vert")),
            *ASSERT(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/4_colored_triangle.frag")),
        };
        ShaderProgram sp{ std::span{ shaders } };
        auto set_alpha_uniform = *ASSERT(sp.bind().make_uniform_setter("u_alpha", glUniform1f));
        return std::make_tuple(std::move(sp), std::move(set_alpha_uniform));
    };

    using buffers_type = std::tuple<
        Buffer<float, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
        Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
        VertexArray>;

    constexpr auto create_triangle_buffers = [](std::span<const float, 3 * (3 + 3)> vertices_w_colors) -> buffers_type {
        VertexArrayBuilder va_builder;
        va_builder.attribute<3, float>();
        va_builder.attribute<3, float>();
        auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vertices_w_colors);
        constexpr std::array<unsigned, 3> indices{ 0u, 1u, 2u };
        auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
        auto va = std::move(va_builder).submit();
        return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
    };

    constexpr std::array<float, 3 * (3 + 3)> vertices_w_colors{
        // positions      | colors
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top
    };

    const std::tuple triangle{
        create_triangle_shader(),
        create_triangle_buffers(std::span{ vertices_w_colors }),
    };

    // VVV debug
    debug_unbind();
    // ^^^ debug

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        const float alpha = static_cast<float>(std::sin(glfwGetTime())) / 2.0f + 0.5f;

        {
            const auto& [shader, buffers] = triangle;
            const auto& [sp, set_alpha_uniform] = shader;
            const auto& [vb, eb, va] = buffers;
            Draw draw(sp, va);
            set_alpha_uniform(draw.get_sp_bind(), alpha);
            draw.elements(eb);
        }

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
