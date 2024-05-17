//
// Created by usatiynyan on 11/28/23.
//

#include "sl/gfx.hpp"

#include <sl/rt.hpp>

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>

namespace gfx = sl::gfx;
namespace rt = sl::rt;

auto create_triangle_shader(const std::filesystem::path& root) {
    const std::array<gfx::shader, 2> shaders{
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/04_colored_triangle.vert")),
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::fragment, root / "shaders/04_colored_triangle.frag")),
    };
    auto sp = *ASSERT_VAL(gfx::shader_program::build(std::span{ shaders }));
    auto set_alpha_uniform = *ASSERT_VAL(sp.bind().make_uniform_setter(glUniform1f, "u_alpha"));
    return std::make_tuple(std::move(sp), std::move(set_alpha_uniform));
};

template <typename T>
using element_buffer = gfx::buffer<T, gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>;

using buffers_type = std::tuple<
    gfx::buffer<float, gfx::buffer_type::array, gfx::buffer_usage::static_draw>,
    element_buffer<unsigned>,
    gfx::vertex_array>;

buffers_type create_triangle_buffers(std::span<const float, 3 * (3 + 3)> vertices_w_colors) {
    gfx::vertex_array_builder va_builder;
    va_builder.attribute<3, float>();
    va_builder.attribute<3, float>();
    auto vb = va_builder.buffer<gfx::buffer_type::array, gfx::buffer_usage::static_draw>(vertices_w_colors);
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
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "04_colored_triangle", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    constexpr std::array<float, 3 * (3 + 3)> vertices_w_colors{
        // positions      | colors
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top
    };

    const std::tuple triangle{
        create_triangle_shader(root),
        create_triangle_buffers(std::span{ vertices_w_colors }),
    };

    // VVV debug
    debug_unbind();
    // ^^^ debug

    while (!current_window.should_close()) {
        ctx->poll_events();

        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        const float alpha = static_cast<float>(std::sin(glfwGetTime())) / 2.0f + 0.5f;

        {
            const auto& [shader, buffers] = triangle;
            const auto& [sp, set_alpha_uniform] = shader;
            const auto& [vb, eb, va] = buffers;
            const auto bound_sp = sp.bind();
            const auto bound_va = va.bind();
            set_alpha_uniform(bound_sp, alpha);
            gfx::draw{ bound_sp, bound_va }.elements(eb);
        }

        current_window.swap_buffers();
    }

    return 0;
}
