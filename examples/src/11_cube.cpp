//
// Created by usatiynyan.
//

#include "sl/gfx.hpp"

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sl::gfx;

struct VT {
    va_attrib_field<3, float> vert;
    va_attrib_field<2, float> tex;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr std::array vertices_w_tex_coords{
    // positions      | texture coords
    // front face
    VT{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f }, // top right
    VT{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f }, // bottom right
    VT{ -0.5f, -0.5f, 0.5f, 0.0f, 0.0f }, // bottom left
    VT{ -0.5f, 0.5f, 0.5f, 0.0f, 1.0f }, // top left
    // right face
    VT{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f }, // top right
    VT{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f }, // bottom right
    VT{ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f }, // bottom left
    VT{ 0.5f, 0.5f, -0.5f, 0.0f, 1.0f }, // top left
    // back face
    VT{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f }, // top right
    VT{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f }, // bottom right
    VT{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f }, // bottom left
    VT{ -0.5f, 0.5f, -0.5f, 0.0f, 1.0f }, // top left
    // left face
    VT{ -0.5f, 0.5f, -0.5f, 1.0f, 1.0f }, // top right
    VT{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f }, // bottom right
    VT{ -0.5f, -0.5f, 0.5f, 0.0f, 0.0f }, // bottom left
    VT{ -0.5f, 0.5f, 0.5f, 0.0f, 1.0f }, // top left
    // top face
    VT{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f }, // top right
    VT{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f }, // bottom right
    VT{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f }, // bottom left
    VT{ -0.5f, 0.5f, 0.5f, 0.0f, 1.0f }, // top left
    // bottom face
    VT{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f }, // top right
    VT{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f }, // bottom right
    VT{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f }, // bottom left
    VT{ -0.5f, -0.5f, 0.5f, 0.0f, 1.0f }, // top left
};
#pragma GCC diagnostic pop

constexpr std::array indices{
    0u,  1u,  3u,  1u,  2u,  3u, // Front face
    4u,  5u,  7u,  5u,  6u,  7u, // Right face
    8u,  9u,  11u, 9u,  10u, 11u, // Back face
    12u, 13u, 15u, 13u, 14u, 15u, // Left face
    16u, 17u, 19u, 17u, 18u, 19u, // Top face
    20u, 21u, 23u, 21u, 22u, 23u, // Bottom face
};

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto ctx = *ASSERT_VAL(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    Size2I window_size{ 800, 600 };
    const auto window = ASSERT_VAL(Window::create(ctx, "11_cube", window_size));
    (void)window->FramebufferSize_cb.connect([&](GLsizei width, GLsizei height) {
        window_size = Size2I{ width, height };
        Window::Current{ *window }.viewport(Vec2I{}, window_size);
    });
    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    constexpr auto create_texture = [](const std::filesystem::path& image_path) {
        TextureBuilder<TextureType::TEXTURE_2D> tex_builder;
        tex_builder.set_wrap_s(TextureWrap::REPEAT);
        tex_builder.set_wrap_t(TextureWrap::REPEAT);
        tex_builder.set_min_filter(TextureFilter::NEAREST);
        tex_builder.set_max_filter(TextureFilter::NEAREST);

        const auto image = *ASSERT_VAL(stb::image_load(image_path, 4));
        tex_builder.set_image(
            std::array{ image.width, image.height }, TextureFormat{ GL_RGB, GL_RGBA }, image.data.get()
        );
        return std::move(tex_builder).submit();
    };

    constexpr auto create_shader = [](std::span<const std::string_view, 2> tex_uniform_names) {
        std::array<const Shader, 2> shaders{
            *ASSERT_VAL(Shader::load_from_file(ShaderType::VERTEX, "shaders/11_cube.vert")),
            *ASSERT_VAL(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/11_cube.frag")),
        };
        auto sp = *ASSERT_VAL(ShaderProgram::build(std::span{ shaders }));
        auto sp_bind = sp.bind();
        sp_bind.initialize_tex_units(tex_uniform_names);
        auto set_transform =
            *ASSERT_VAL(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
        return std::make_tuple(std::move(sp), std::move(set_transform));
    };

    using buffers_type = std::tuple<
        Buffer<VT, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
        Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
        VertexArray>;

    constexpr auto create_buffers = //
        [](std::span<const VT, 24> vertices_w_tex_coords) -> buffers_type {
        VertexArrayBuilder va_builder;
        va_builder.attributes_from<VT>();
        auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vertices_w_tex_coords);
        auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
        auto va = std::move(va_builder).submit();
        return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
    };

    const std::tuple texs{ create_texture("textures/cosmos.jpg"), create_texture("textures/osaka.jpg") };
    const std::array<std::string_view, 2> tex_uniform_names{ "us_texture_bg", "us_texture_fg" };
    const auto shader = create_shader(std::span{ tex_uniform_names });
    const auto buffers = create_buffers(std::span{ vertices_w_tex_coords });

    current_window.enable(GL_DEPTH_TEST);

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            const auto& [vb, eb, va] = buffers;
            const auto& [sp, set_transform] = shader;

            Draw draw(sp, va, texs);

            const auto time = static_cast<float>(glfwGetTime());

            constexpr float fov = glm::radians(45.0f);
            const glm::mat4 projection = glm::perspective(fov, window_size.aspect_ratio(), 0.1f, 100.0f);
            const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
            const glm::mat4 model =
                glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

            const glm::mat4 transform = projection * view * model; // leaning osaker
            set_transform(draw.sp_bind(), glm::value_ptr(transform));
            draw.elements(eb);
        }

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
