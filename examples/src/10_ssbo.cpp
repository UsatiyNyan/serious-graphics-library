//
// Created by usatiynyan on 12/2/23.
//

#include "sl/gfx.hpp"

#include <assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace sl::gfx;

struct Vert {
    va_attrib_field<2, float> pos;
};

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto ctx = *ASSERT(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    const Size2I window_size{ 800, 600 };
    const auto window = ASSERT(Window::create(ctx, "10_ssbo", window_size));
    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });

    constexpr auto create_shader = [] {
        std::array<const Shader, 2> shaders{
            *ASSERT(Shader::load_from_file(ShaderType::VERTEX, "shaders/10_ssbo.vert")),
            *ASSERT(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/10_ssbo.frag")),
        };
        ShaderProgram sp{ std::span{ shaders } };
        auto sp_bind = sp.bind();
        auto set_transform = *ASSERT(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
        auto set_window_size = *ASSERT(sp_bind.make_uniform_setter(glUniform2f, "u_window_size"));
        return std::make_tuple(std::move(sp), std::move(set_transform), std::move(set_window_size));
    };

    constexpr auto create_buffers = [](std::span<const Vert, 4> vertices) {
        VertexArrayBuilder va_builder;
        va_builder.attributes_from<Vert>();
        auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vertices);
        constexpr std::array<unsigned, 6> indices{
            0u, 1u, 3u, // first triangle
            1u, 2u, 3u, // second triangle
        };
        auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
        auto va = std::move(va_builder).submit();
        return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
    };

    constexpr auto create_ssbo = [] {
        Buffer<float, BufferType::SHADER_STORAGE, BufferUsage::DYNAMIC_DRAW> ssbo;
        auto ssbo_bind = ssbo.bind();
        ssbo_bind.base<0>();
        ssbo_bind.initialize_data<1024>();
        return ssbo;
    };

    constexpr std::array vertices{
        Vert{ +1.0f, +1.0f },
        Vert{ +1.0f, -1.0f },
        Vert{ -1.0f, -1.0f },
        Vert{ -1.0f, +1.0f },
    };

    const auto [sp, set_transform, set_window_size] = create_shader();
    const auto [vb, eb, va] = create_buffers(vertices);
    auto ssbo = create_ssbo();

    // init
    {
        const auto sp_bind = sp.bind();
        const auto transform = glm::mat4(1.0f);
        set_transform(sp_bind, glm::value_ptr(transform));
        set_window_size(sp.bind(), static_cast<float>(window_size.width), static_cast<float>(window_size.height));

        window->FramebufferSize_cb = [&window,
                                      sp_ref = std::ref(sp),
                                      set_window_size_ref = std::ref(set_window_size)](GLsizei width, GLsizei height) {
            Window::Current{ *window }.viewport(Vec2I{}, Size2I{ width, height });
            set_window_size_ref(sp_ref.get().bind(), static_cast<float>(width), static_cast<float>(height));
        };
    }

    while (!current_window.should_close()) {
        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }
        current_window.clear(GL_COLOR_BUFFER_BIT);

        {
            Draw draw(sp, va, {});
            {
                auto mapped_ssbo = ssbo.bind().map<BufferAccess::WRITE_ONLY, 1024>();
                auto mapped_ssbo_data = mapped_ssbo.data();
                const float current_time = static_cast<float>(glfwGetTime());
                for (std::size_t i = 0; i != mapped_ssbo_data.size(); ++i) {
                    float& mapped_ssbo_datum = mapped_ssbo_data[i];
                    const float angle =
                        (static_cast<float>(i) / mapped_ssbo_data.size()) * 2 * std::numbers::pi_v<float>;
                    mapped_ssbo_datum = std::sin(current_time - angle);
                }
            }
            draw.elements(eb);
        }

        current_window.swap_buffers();
        ctx.poll_events();
    }

    return 0;
}
