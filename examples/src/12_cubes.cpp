//
// Created by usatiynyan.
//

#include "sl/gfx.hpp"
#include "sl_example.hpp"

#include <sl/meta/match/pmatch.hpp>
#include <sl/rt.hpp>

#include <chrono>
#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <utility>

namespace gfx = sl::gfx;
namespace rt = sl::rt;

auto create_texture(const std::filesystem::path& image_path) {
    gfx::texture_builder tex_builder{ gfx::texture_type::texture_2d };
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
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::vertex, root / "shaders/12_cubes.vert")),
        *ASSERT_VAL(gfx::shader::load_from_file(gfx::shader_type::fragment, root / "shaders/12_cubes.frag")),
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
    VT{ +0.5f, +0.5f, +0.5f, +1.0f, +1.0f }, // top right
    VT{ +0.5f, -0.5f, +0.5f, +1.0f, +0.0f }, // bottom right
    VT{ -0.5f, -0.5f, +0.5f, +0.0f, +0.0f }, // bottom left
    VT{ -0.5f, +0.5f, +0.5f, +0.0f, +1.0f }, // top left
    // right face
    VT{ +0.5f, +0.5f, +0.5f, +1.0f, +1.0f }, // top right
    VT{ +0.5f, -0.5f, +0.5f, +1.0f, +0.0f }, // bottom right
    VT{ +0.5f, -0.5f, -0.5f, +0.0f, +0.0f }, // bottom left
    VT{ +0.5f, +0.5f, -0.5f, +0.0f, +1.0f }, // top left
    // back face
    VT{ +0.5f, +0.5f, -0.5f, +1.0f, +1.0f }, // top right
    VT{ +0.5f, -0.5f, -0.5f, +1.0f, +0.0f }, // bottom right
    VT{ -0.5f, -0.5f, -0.5f, +0.0f, +0.0f }, // bottom left
    VT{ -0.5f, +0.5f, -0.5f, +0.0f, +1.0f }, // top left
    // left face
    VT{ -0.5f, +0.5f, -0.5f, +1.0f, +1.0f }, // top right
    VT{ -0.5f, -0.5f, -0.5f, +1.0f, +0.0f }, // bottom right
    VT{ -0.5f, -0.5f, +0.5f, +0.0f, +0.0f }, // bottom left
    VT{ -0.5f, +0.5f, +0.5f, +0.0f, +1.0f }, // top left
    // top face
    VT{ +0.5f, +0.5f, +0.5f, +1.0f, +1.0f }, // top right
    VT{ +0.5f, +0.5f, -0.5f, +1.0f, +0.0f }, // bottom right
    VT{ -0.5f, +0.5f, -0.5f, +0.0f, +0.0f }, // bottom left
    VT{ -0.5f, +0.5f, +0.5f, +0.0f, +1.0f }, // top left
    // bottom face
    VT{ +0.5f, -0.5f, +0.5f, +1.0f, +1.0f }, // top right
    VT{ +0.5f, -0.5f, -0.5f, +1.0f, +0.0f }, // bottom right
    VT{ -0.5f, -0.5f, -0.5f, +0.0f, +0.0f }, // bottom left
    VT{ -0.5f, -0.5f, +0.5f, +0.0f, +1.0f }, // top left
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

constexpr std::array cube_positions{
    glm::vec3{ 0.0f, 0.0f, 0.0f }, //
    glm::vec3{ 2.0f, 5.0f, -15.0f }, //
    glm::vec3{ -1.5f, -2.2f, -2.5f }, //
    glm::vec3{ -3.8f, -2.0f, -12.3f }, //
    glm::vec3{ 2.4f, -0.4f, -3.5f }, //
    glm::vec3{ -1.7f, 3.0f, -7.5f }, //
    glm::vec3{ 1.3f, -2.0f, -2.5f }, //
    glm::vec3{ 1.5f, 2.0f, -2.5f }, //
    glm::vec3{ 1.5f, 0.2f, -1.5f }, //
    glm::vec3{ -1.3f, 1.0f, -1.5f }, //
};

int main(int argc, char** argv) {
    const rt::context rt_ctx{ argc, argv };
    const auto root = rt_ctx.path().parent_path();

    spdlog::set_level(spdlog::level::debug);

    auto ctx = ASSERT_VAL(gfx::context::create(gfx::context::options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    constexpr glm::ivec2 window_size{ 800, 600 };
    const auto window = ASSERT_VAL(gfx::window::create(*ctx, "12_cubes", window_size));

    (void)window->frame_buffer_size_cb.connect([&window](glm::ivec2 size) {
        gfx::current_window{ *window }.viewport(glm::ivec2{}, size);
    });

    constexpr glm::fvec4 clear_color{ 0.2f, 0.3f, 0.3f, 1.0f };
    auto current_window = window->make_current(*ctx, glm::ivec2{}, window_size, clear_color);

    auto prev_update_time = std::chrono::steady_clock::now();

    constexpr gfx::basis world{};

    gfx::camera camera{
        .tf{
            .tr = glm::vec3{ 0.0f, 0.0f, 3.0f },
            .rot = glm::angleAxis(glm::radians(-180.0f), world.up()),
        },
        .proj{ gfx::perspective_projection{
            .fov = glm::radians(45.0f),
            .near = 0.1f,
            .far = 100.0f,
        } },
    };

    const auto transform_from_keyboard = [&world](const gfx::current_window& cw) {
        gfx::transform movement{};
        if (cw.is_key_pressed(GLFW_KEY_W)) {
            movement.translate(world.forward());
        }
        if (cw.is_key_pressed(GLFW_KEY_S)) {
            movement.translate(-world.forward());
        }
        if (cw.is_key_pressed(GLFW_KEY_D)) {
            movement.translate(world.right());
        }
        if (cw.is_key_pressed(GLFW_KEY_A)) {
            movement.translate(-world.right());
        }
        if (cw.is_key_pressed(GLFW_KEY_Q)) {
            movement.translate(world.up());
        }
        if (cw.is_key_pressed(GLFW_KEY_E)) {
            movement.translate(-world.up());
        }
        return normalize(movement);
    };

    current_window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    std::optional<glm::dvec2> last_cursor_pos{};

    (void)window->cursor_pos_cb.connect([&](glm::dvec2 cursor_pos) {
        if (!last_cursor_pos.has_value()) {
            last_cursor_pos = cursor_pos;
            return;
        }
        const glm::vec2 cursor_offset = *last_cursor_pos - cursor_pos;
        last_cursor_pos = cursor_pos;

        constexpr float sensitivity = glm::radians(0.1f);
        const float yaw = cursor_offset.x * sensitivity;
        const float pitch = cursor_offset.y * sensitivity;

        const glm::vec3 camera_forward = camera.tf.rot * world.forward();
        const glm::vec3 camera_right = glm::cross(camera_forward, world.up());
        const glm::quat rot_x = glm::angleAxis(pitch, camera_right);
        const glm::quat rot_y = glm::angleAxis(yaw, world.y);

        camera.tf.rotate(rot_x * rot_y);
    });

    (void)window->scroll_cb.connect([&](glm::dvec2 scroll_offset) {
        camera.proj.value
            | sl::meta::pmatch{
                  [yoffset = static_cast<float>(scroll_offset.y)](gfx::perspective_projection& x) {
                      constexpr float zoom_sensitivity = 0.1f;
                      const float zoom = yoffset * zoom_sensitivity;
                      x.fov = glm::clamp(x.fov - zoom, glm::radians(0.1f), glm::radians(90.0f));
                      spdlog::info("fov: {}", glm::degrees(x.fov));
                  },
                  [](auto&) {} // skip ortho
              };
    });

    const auto update = [&](float delta_time, const gfx::transform& movement) {
        constexpr float camera_acc = 2.5f;
        const float camera_speed = camera_acc * delta_time;
        camera.tf.translate(camera_speed * (camera.tf.rot * movement.tr));
    };

    // prepare render
    const std::array<gfx::texture, 2> textures{
        create_texture(root / "textures/cosmos.jpg"),
        create_texture(root / "textures/osaka.jpg"),
    };
    const auto shader = create_shader(root);
    const auto buffers = create_buffers(std::span{ vertices_w_tex_coords }, std::span{ indices });
    current_window.enable(GL_DEPTH_TEST);

    while (!current_window.should_close()) {
        // input
        ctx->poll_events();

        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        const gfx::transform movement = transform_from_keyboard(current_window);

        // time
        const float delta_time = [&prev_update_time] {
            const auto curr_update_time = std::chrono::steady_clock::now();
            const auto delta_update_time = curr_update_time - std::exchange(prev_update_time, curr_update_time);
            return std::chrono::duration<float>(delta_update_time).count();
        }();

        // update
        {
            update(delta_time, movement); //
        }

        // render
        {
            current_window.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const auto& [vb, eb, va] = buffers;
            const auto& [sp, set_transform] = shader;

            const auto bound_sp = sp.bind();
            const auto bound_va = va.bind();
            const auto bound_texs = gfx::activate_textures(textures);
            gfx::draw draw{ bound_sp, bound_va, bound_texs };

            const glm::mat4 projection = camera.projection(window_size);
            const glm::mat4 view = camera.view(world);

            for (const auto& [index, pos] : ranges::views::enumerate(cube_positions)) {
                const float angle = 20.0f * (static_cast<float>(index));
                const glm::mat4 model = glm::rotate( // 1st op
                    glm::translate(glm::mat4(1.0f), pos), // 2nd op
                    glm::radians(angle),
                    glm::vec3(1.0f, 0.3f, 0.5f)
                );
                const glm::mat4 transform = projection * view * model;
                set_transform(bound_sp, glm::value_ptr(transform));
                draw.elements(eb);
            }

            current_window.swap_buffers();
        }
    }

    return 0;
}
