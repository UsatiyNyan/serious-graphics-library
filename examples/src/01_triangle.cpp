//
// Created by usatiynyan on 11/18/23.
//

#include "sl/gfx.hpp"

#include <sl/rt.hpp>

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>

namespace gfx = sl::gfx;
namespace rt = sl::rt;

void debug_unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

template <typename T, std::size_t extent>
void draw(const gfx::shader_program& sp, const gfx::vertex_array& va, std::span<const T, extent> vertices) {
    const auto sp_bind [[maybe_unused]] = sp.bind();
    const auto va_bind [[maybe_unused]] = va.bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

int main(int argc, char** argv) {
    const rt::context rt_ctx{ argc, argv };
    const auto root = rt_ctx.path().parent_path();

    spdlog::set_level(spdlog::level::debug);

    auto ctx = ASSERT_VAL(gfx::context::create(gfx::context::options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "01_triangle", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    const std::array shaders{
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/01_triangle.vert")),
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::fragment, root / "shaders/01_triangle.frag")),
    };
    const auto sp = *ASSERT_VAL(gfx::shader_program::build(std::span{ shaders }));

    gfx::vertex_array_builder va_builder;
    va_builder.attribute<3, float>();
    constexpr std::array<float, 3 * 3> vertices{
        -0.5f, -0.5f, 0.0f, //
        0.5f,  -0.5f, 0.0f, //
        0.0f,  0.5f,  0.0f, //
    };
    const auto vb = va_builder.buffer<gfx::buffer_type::array, gfx::buffer_usage::static_draw>(std::span{ vertices });
    const auto va = std::move(va_builder).submit();

    // VVV debug
    debug_unbind();
    // ^^^ debug

    while (!current_window.should_close()) {
        ctx->poll_events();
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        current_window.clear(GL_COLOR_BUFFER_BIT);

        draw(sp, va, std::span{ vertices });

        current_window.swap_buffers();
    }

    return 0;
}
