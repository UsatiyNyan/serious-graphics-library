//

#include "sl/gfx.hpp"

#include <sl/rt.hpp>

#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>

namespace gfx = sl::gfx;
namespace rt = sl::rt;

auto create_triangle_shader(const std::filesystem::path& root, glm::fvec4 color) {
    const auto fragment_shader_source = fmt::format(
        "#version 460 core\n"
        "out vec4 FragColor;\n"
        "void main() {{\n"
        "    FragColor = vec4({});"
        "\n}}",
        fmt::join(std::array{ color.r, color.g, color.b, color.a }, ", ")
    );
    const std::array<gfx::shader, 2> shaders{
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/03_two_triangles.vert")),
        *ASSERT_VAL(gfx::shader::load_from_source(gfx::shader_type::fragment, fragment_shader_source)),
    };
    return *ASSERT_VAL(gfx::shader_program::build(std::span{ shaders }));
};

template <typename T>
using element_buffer = gfx::buffer<T, gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>;

using buffers_type = std::tuple<
    gfx::buffer<float, gfx::buffer_type::array, gfx::buffer_usage::static_draw>,
    element_buffer<unsigned>,
    gfx::vertex_array>;

auto create_triangle_buffers(std::span<const float, 3 * 3> vertices) -> buffers_type {
    gfx::vertex_array_builder va_builder;
    va_builder.attribute<3, float>();
    auto vb = va_builder.buffer<gfx::buffer_type::array, gfx::buffer_usage::static_draw>(vertices);
    constexpr std::array<unsigned, 3> indices{ 0u, 1u, 2u };
    auto eb = va_builder.buffer<gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>(std::span{ indices });
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

void debug_unbind() {
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename T>
void draw(const gfx::shader_program& sp, const gfx::vertex_array& va, const element_buffer<T>& eb) {
    [[maybe_unused]] const auto sp_bind = sp.bind();
    const auto va_bind = va.bind();
    glDrawElements(GL_TRIANGLES, eb.data_size(), gfx::gl_type_query<T>, nullptr);
}

int main(int argc, char** argv) {
    const rt::context rt_ctx{ argc, argv };
    const auto root = rt_ctx.path().parent_path();

    spdlog::set_level(spdlog::level::debug);

    auto ctx = ASSERT_VAL(gfx::context::create(gfx::context::options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "03_two_triangles", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    const std::array triangles{
        std::pair{
            create_triangle_shader(root, glm::fvec4{ 1.0f, 0.5f, 0.2f, 1.0f }),
            create_triangle_buffers(std::span{ left_triangle_vertices }),
        },
        std::pair{
            create_triangle_shader(root, glm::fvec4{ 0.7f, 0.1f, 0.7f, 1.0f }),
            create_triangle_buffers(std::span{ right_triangle_vertices }),
        },
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

        for (const std::pair<gfx::shader_program, buffers_type>& triangle : triangles) {
            const auto& [sp, buffers] = triangle;
            const auto& [vb, eb, va] = buffers;
            draw(sp, va, eb);
        }

        current_window.swap_buffers();
    }

    return 0;
}
