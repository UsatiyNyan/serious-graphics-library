//
// Created by usatiynyan on 12/2/23.
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

auto create_shader(const std::filesystem::path& root) {
    const std::array<gfx::shader, 2> shaders{
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/10_ssbo.vert")),
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::fragment, root / "shaders/10_ssbo.frag")),
    };
    auto sp = *ASSERT_VAL(gfx::shader_program::build(std::span{ shaders }));
    auto sp_bind = sp.bind();
    auto set_transform = *ASSERT_VAL(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
    auto set_window_size = *ASSERT_VAL(sp_bind.make_uniform_setter(glUniform2f, "u_window_size"));
    return std::make_tuple(std::move(sp), std::move(set_transform), std::move(set_window_size));
};

struct V {
    gfx::va_attrib_field<2, float> pos;
};

auto create_buffers(std::span<const V, 4> vertices) {
    gfx::vertex_array_builder va_builder;
    va_builder.attributes_from<V>();
    auto vb = va_builder.buffer<gfx::buffer_type::array, gfx::buffer_usage::static_draw>(vertices);
    constexpr std::array<unsigned, 6> indices{
        0u, 1u, 3u, // first triangle
        1u, 2u, 3u, // second triangle
    };
    auto eb = va_builder.buffer<gfx::buffer_type::element_array, gfx::buffer_usage::static_draw>(std::span{ indices });
    auto va = std::move(va_builder).submit();
    return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
};

auto create_ssbo() {
    gfx::buffer<float, gfx::buffer_type::shader_storage, gfx::buffer_usage::dynamic_draw> ssbo;
    auto ssbo_bind = ssbo.bind();
    ssbo_bind.bind_base<0>();
    ssbo_bind.initialize_data<1024>();
    return ssbo;
};

int main(int argc, char** argv) {
    const rt::context rt_ctx{ argc, argv };
    const auto root = rt_ctx.path().parent_path();

    spdlog::set_level(spdlog::level::debug);

    auto ctx = ASSERT_VAL(gfx::context::create(gfx::context::options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "10_ssbo", window_size));

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    constexpr std::array vertices{
        V{ +1.0f, +1.0f },
        V{ +1.0f, -1.0f },
        V{ -1.0f, -1.0f },
        V{ -1.0f, +1.0f },
    };

    const auto [sp, set_transform, set_window_size] = create_shader(root);
    const auto [vb, eb, va] = create_buffers(vertices);
    auto ssbo = create_ssbo();

    // init
    {
        const auto sp_bind = sp.bind();
        const auto transform = glm::mat4(1.0f);
        set_transform(sp_bind, glm::value_ptr(transform));
        set_window_size(sp.bind(), static_cast<float>(window_size.x), static_cast<float>(window_size.y));

        (void)window->frame_buffer_size_cb.connect(
            [&window, sp_ref = std::ref(sp), set_window_size_ref = std::ref(set_window_size)](glm::ivec2 size) {
                gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
                set_window_size_ref(sp_ref.get().bind(), static_cast<float>(size.x), static_cast<float>(size.y));
            }
        );
    }

    while (!current_window.should_close()) {
        ctx->poll_events();
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        {
            gfx::draw draw{ sp.bind(), va.bind() };
            {
                auto mapped_ssbo = *ASSERT_VAL((ssbo.bind().map<gfx::buffer_access::write_only, 1024>()));
                auto mapped_ssbo_data = mapped_ssbo.data();
                const float current_time = static_cast<float>(glfwGetTime());
                for (std::size_t i = 0; i != mapped_ssbo_data.size(); ++i) {
                    const float angle =
                        (static_cast<float>(i) / mapped_ssbo_data.size()) * 2 * std::numbers::pi_v<float>;
                    mapped_ssbo_data[i] = std::sin(current_time - angle);
                }
            }
            draw.elements(eb);
        }

        current_window.swap_buffers();
    }

    return 0;
}
