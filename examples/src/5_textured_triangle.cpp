//
// Created by usatiynyan on 11/29/23.
//

#include "sl/gfx.hpp"

#include <assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

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

    constexpr auto create_triangle_texture = []() {
        TextureBuilder<TextureType::TEXTURE_2D> tex_builder;
        tex_builder.set_wrap_s(TextureWrap::REPEAT);
        tex_builder.set_wrap_t(TextureWrap::REPEAT);
        tex_builder.set_min_filter(TextureFilter::NEAREST);
        tex_builder.set_max_filter(TextureFilter::NEAREST);

        const auto image = *ASSERT(stb::image_load("textures/osaka.jpg", 4));
        tex_builder.set_image(
            std::array{ image.width, image.height }, TextureFormat{ GL_RGB, GL_RGBA }, image.data.get()
        );
        return std::move(tex_builder).submit();
    };

    constexpr auto create_triangle_shader = [] {
        std::array<const Shader, 2> shaders{
            *ASSERT(Shader::load_from_file(ShaderType::VERTEX, "shaders/5_textured_triangle.vert")),
            *ASSERT(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/5_textured_triangle.frag")),
        };
        return ShaderProgram{ std::span{ shaders } };
    };

    using buffers_type = std::tuple<
        Buffer<float, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
        Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
        VertexArray>;

    constexpr auto create_triangle_buffers = //
        [](std::span<const float, 3 * (3 + 2)> vertices_w_tex_coords) -> buffers_type {
        VertexArrayBuilder va_builder;
        va_builder.attribute<3, float>();
        va_builder.attribute<2, float>();
        auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vertices_w_tex_coords);
        constexpr std::array<unsigned, 3> indices{ 0u, 1u, 2u };
        auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
        auto va = std::move(va_builder).submit();
        return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
    };

    constexpr std::array<float, 3 * (3 + 2)> vertices_w_tex_coords{
        // positions      | tex coords
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        0.0f,  0.5f,  0.0f, 0.5f, 1.0f, // top
    };


    const std::tuple triangle{
        create_triangle_texture(),
        create_triangle_shader(),
        create_triangle_buffers(std::span{ vertices_w_tex_coords }),
    };

    // VVV debug
    debug_unbind();
    // ^^^ debug

    constexpr int TEXTURE_UNIT = 0;

    {
        // NOTE: need to set texture uniforms only once
        const auto& [tex, sp, buffers] = triangle;
        const auto sp_bind = sp.bind();
        const auto set_texture_unit = *ASSERT(sp_bind.make_uniform_setter("us_texture", glUniform1i));
        set_texture_unit(sp_bind, TEXTURE_UNIT);
    }

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        {
            const auto& [tex, sp, buffers] = triangle;
            const auto& [vb, eb, va] = buffers;
            const auto tex_bind = tex.activate<TEXTURE_UNIT>();
            Draw draw(sp, va);
            draw.elements(eb);
        }

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
