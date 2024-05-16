//
// Created by usatiynyan on 11/29/23.
//

#include "sl/gfx.hpp"

#include <sl/rt.hpp>

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

namespace gfx = sl::gfx;
namespace rt = sl::rt;

auto create_triangle_texture(const std::filesystem::path& root) {
    gfx::texture_builder<gfx::texture_type::texture_2d> tex_builder;
    tex_builder.set_wrap_s(gfx::texture_wrap::repeat);
    tex_builder.set_wrap_t(gfx::texture_wrap::repeat);
    tex_builder.set_min_filter(gfx::texture_filter::nearest);
    tex_builder.set_max_filter(gfx::texture_filter::nearest);

    const auto image = *ASSERT_VAL(stb::image_load(root / "textures/osaka.jpg", 4));
    tex_builder.set_image(std::span{ image.dimensions }, gfx::texture_format{ GL_RGB, GL_RGBA }, image.data.get());
    return std::move(tex_builder).submit();
};

auto create_triangle_shader(const std::filesystem::path& root) {
    const std::array<gfx::shader, 2> shaders{
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/05_textured_triangle.vert")),
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::fragment, root / "shaders/05_textured_triangle.frag")
        ),
    };
    return *ASSERT_VAL(gfx::shader_program::build(std::span{ shaders }));
};

using buffers_type = std::tuple<
    gfx::buffer<float, gfx::buffer_type::array, gfx::buffer_usage::static_draw>,
    gfx::buffer<unsigned, gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>,
    gfx::vertex_array>;

buffers_type create_triangle_buffers(std::span<const float, 3 * (3 + 2)> vertices_w_tex_coords) {
    gfx::vertex_array_builder va_builder;
    va_builder.attribute<3, float>();
    va_builder.attribute<2, float>();
    auto vb = va_builder.buffer<gfx::buffer_type::array, gfx::buffer_usage::static_draw>(vertices_w_tex_coords);
    constexpr std::array<unsigned, 3> indices{ 0u, 1u, 2u };
    auto eb = va_builder.buffer<gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>(std::span{ indices });
    auto va = std::move(va_builder).submit();
    return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
};

void debug_unbind() {
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int main(int argc, char** argv) {
    const rt::context rt_ctx{ argc, argv };
    const auto root = rt_ctx.path().parent_path();

    spdlog::set_level(spdlog::level::debug);

    auto ctx = ASSERT_VAL(gfx::context::create(gfx::context::options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "05_textured_triangle", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    constexpr std::array<float, 3 * (3 + 2)> vertices_w_tex_coords{
        // positions      | tex coords
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        0.0f,  0.5f,  0.0f, 0.5f, 1.0f, // top
    };

    const std::tuple triangle{
        create_triangle_texture(root),
        create_triangle_shader(root),
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
        const auto set_texture_unit = *ASSERT_VAL(sp_bind.make_uniform_setter(glUniform1i, "us_texture"));
        set_texture_unit(sp_bind, TEXTURE_UNIT);
    }

    while (!current_window.should_close()) {
        ctx->poll_events();

        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        current_window.clear(GL_COLOR_BUFFER_BIT);

        {
            const auto& [tex, sp, buffers] = triangle;
            const auto& [vb, eb, va] = buffers;
            const auto tex_bind = tex.activate<TEXTURE_UNIT>();
            gfx::draw draw{ sp.bind(), va.bind() };
            draw.elements(eb);
        }

        current_window.swap_buffers();
    }

    return 0;
}
