//
// Created by usatiynyan.
//

#include "sl/gfx.hpp"

#include <sl/rt.hpp>

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gfx = sl::gfx;
namespace rt = sl::rt;

auto create_texture(const std::filesystem::path& image_path) {
    gfx::texture_builder<gfx::texture_type::texture_2d> tex_builder;
    tex_builder.set_wrap_s(gfx::texture_wrap::repeat);
    tex_builder.set_wrap_t(gfx::texture_wrap::repeat);
    tex_builder.set_min_filter(gfx::texture_filter::nearest);
    tex_builder.set_max_filter(gfx::texture_filter::nearest);

    const auto image = *ASSERT_VAL(stb::image_load(image_path, 4));
    tex_builder.set_image(std::span{ image.dimensions }, gfx::texture_format{ GL_RGB, GL_RGBA }, image.data.get());
    return std::move(tex_builder).submit();
};

auto create_shader(const std::filesystem::path& root) {
    const std::array<gfx::shader, 2> shaders{
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/11_cube.vert")),
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::fragment, root / "shaders/11_cube.frag")),
    };
    auto sp = *ASSERT_VAL(gfx::shader_program::build(std::span{ shaders }));
    auto sp_bind = sp.bind();
    constexpr std::array<std::string_view, 2> tex_uniform_names{ "us_texture_bg", "us_texture_fg" };
    sp_bind.initialize_tex_units(std::span{ tex_uniform_names });
    auto set_transform = *ASSERT_VAL(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
    return std::make_tuple(std::move(sp), std::move(set_transform));
};

struct VT {
    gfx::va_attrib_field<3, float> vert;
    gfx::va_attrib_field<2, float> tex;
};

auto create_buffers(std::span<const VT, 24> vertices_w_tex_coords, std::span<const unsigned, 36> indices) {
    gfx::vertex_array_builder va_builder;
    va_builder.attributes_from<VT>();
    auto vb = va_builder.buffer<gfx::buffer_type::array, gfx::buffer_usage::static_draw>(vertices_w_tex_coords);
    auto eb = va_builder.buffer<gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>(indices);
    auto va = std::move(va_builder).submit();
    return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
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

int main(int argc, char** argv) {
    const rt::context rt_ctx{ argc, argv };
    const auto root = rt_ctx.path().parent_path();

    spdlog::set_level(spdlog::level::debug);

    auto ctx = ASSERT_VAL(gfx::context::create(gfx::context::options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "11_cube", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    const auto cosmos_texture = create_texture(root / "textures/cosmos.jpg");
    const auto osaka_texture = create_texture(root / "textures/osaka.jpg");
    const auto shader = create_shader(root);
    const auto buffers = create_buffers(std::span{ vertices_w_tex_coords }, std::span{ indices });

    current_window.enable(GL_DEPTH_TEST);

    while (!current_window.should_close()) {
        ctx->poll_events();

        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        current_window.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {
            const auto& [vb, eb, va] = buffers;
            const auto& [sp, set_transform] = shader;

            gfx::draw draw{ sp.bind(), va, cosmos_texture, osaka_texture };

            const auto time = static_cast<float>(glfwGetTime());

            constexpr float fov = glm::radians(45.0f);
            const glm::mat4 projection = glm::perspective(fov, gfx::aspect_ratio(window_size), 0.1f, 100.0f);
            const glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
            const glm::mat4 model =
                glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

            const glm::mat4 transform = projection * view * model; // leaning osaker
            set_transform(draw.sp(), glm::value_ptr(transform));
            draw.elements(eb);
        }
        current_window.swap_buffers();
    }

    return 0;
}
