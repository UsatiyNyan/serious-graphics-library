//
// Created by usatiynyan on 12/2/23.
//

#include "sl/gfx.hpp"

#include <assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

using namespace sl::gfx;

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto ctx = *ASSERT(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    const Size2I window_size{ 800, 600 };
    const auto window = ASSERT(Window::create(ctx, "6_rectangle_texture_mix", window_size));
    window->FramebufferSize_cb = [&window](GLsizei width, GLsizei height) {
        Window::Current{ *window }.viewport(Vec2I{}, Size2I{ width, height });
    };
    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    constexpr auto create_texture = [](const std::filesystem::path& image_path) {
        TextureBuilder<TextureType::TEXTURE_2D> tex_builder;
        tex_builder.set_wrap_s(TextureWrap::REPEAT);
        tex_builder.set_wrap_t(TextureWrap::REPEAT);
        tex_builder.set_min_filter(TextureFilter::NEAREST);
        tex_builder.set_max_filter(TextureFilter::NEAREST);

        const auto image = *ASSERT(stb::image_load(image_path, 4));
        tex_builder.set_image(
            std::array{ image.width, image.height }, TextureFormat{ GL_RGB, GL_RGBA }, image.data.get()
        );
        return std::move(tex_builder).submit();
    };

    constexpr auto create_shader = [] {
        std::array<const Shader, 2> shaders{
            *ASSERT(Shader::load_from_file(ShaderType::VERTEX, "shaders/6_rectangle_texture_mix.vert")),
            *ASSERT(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/6_rectangle_texture_mix.frag")),
        };
        return ShaderProgram{ std::span{ shaders } };
    };

    using buffers_type = std::tuple<
        Buffer<float, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
        Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
        VertexArray>;

    constexpr auto create_buffers = //
        [](std::span<const float, 4 * (3 + 2)> vertices_w_tex_coords) -> buffers_type {
        VertexArrayBuilder va_builder;
        va_builder.attribute<3, float>();
        va_builder.attribute<2, float>();
        auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vertices_w_tex_coords);
        constexpr std::array<unsigned, 6> indices{
            0u, 1u, 3u, // first triangle
            1u, 2u, 3u, // second triangle
        };
        auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
        auto va = std::move(va_builder).submit();
        return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
    };

    constexpr std::array<float, 4 * (3 + 2)> vertices_w_tex_coords{
        // positions        | texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f // top left
    };

    const std::tuple texs{ create_texture("textures/cosmos.jpg"), create_texture("textures/osaka.jpg") };
    std::array<std::string_view, 2> tex_uniform_names{ "us_texture_bg", "us_texture_fg" };

    const auto sp = create_shader();
    sp.bind().initialize_tex_units(std::span{ tex_uniform_names });

    const auto buffers = create_buffers(std::span{ vertices_w_tex_coords });

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        {
            const auto& [vb, eb, va] = buffers;
            Draw draw(sp, va, texs);
            draw.elements(eb);
        }

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
