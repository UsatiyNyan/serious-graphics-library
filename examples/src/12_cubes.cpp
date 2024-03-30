//
// Created by usatiynyan.
//

#include "sl/gfx.hpp"

#include <chrono>
#include <libassert/assert.hpp>
#include <spdlog/spdlog.h>
#include <stb/image.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <utility>
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

auto create_texture(const std::filesystem::path& image_path) {
    TextureBuilder<TextureType::TEXTURE_2D> tex_builder;
    tex_builder.set_wrap_s(TextureWrap::REPEAT);
    tex_builder.set_wrap_t(TextureWrap::REPEAT);
    tex_builder.set_min_filter(TextureFilter::NEAREST);
    tex_builder.set_max_filter(TextureFilter::NEAREST);

    const auto image = *ASSERT_VAL(stb::image_load(image_path, 4));
    tex_builder.set_image(std::array{ image.width, image.height }, TextureFormat{ GL_RGB, GL_RGBA }, image.data.get());
    return std::move(tex_builder).submit();
};

auto create_shader(std::span<const std::string_view, 2> tex_uniform_names) {
    std::array<const Shader, 2> shaders{
        *ASSERT_VAL(Shader::load_from_file(ShaderType::VERTEX, "shaders/11_cube.vert")),
        *ASSERT_VAL(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/11_cube.frag")),
    };
    auto sp = *ASSERT_VAL(ShaderProgram::build(std::span{ shaders }));
    auto sp_bind = sp.bind();
    sp_bind.initialize_tex_units(tex_uniform_names);
    auto set_transform = *ASSERT_VAL(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
    return std::make_tuple(std::move(sp), std::move(set_transform));
};

using buffers_type = std::tuple<
    Buffer<VT, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
    Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
    VertexArray>;

buffers_type create_buffers(std::span<const VT, 24> vt_coords) {
    VertexArrayBuilder va_builder;
    va_builder.attributes_from<VT>();
    auto vb = va_builder.buffer<BufferType::ARRAY, BufferUsage::STATIC_DRAW>(vt_coords);
    auto eb = va_builder.buffer<BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>(std::span{ indices });
    auto va = std::move(va_builder).submit();
    return std::make_tuple(std::move(vb), std::move(eb), std::move(va));
};

int main() {
    spdlog::set_level(spdlog::level::info);

    auto ctx = *ASSERT_VAL(Context::create(Context::Options{ 4, 6, GLFW_OPENGL_CORE_PROFILE }));
    Size2I window_size{ 800, 600 };
    const auto window = ASSERT_VAL(Window::create(ctx, "12_cubes", window_size));
    (void)window->FramebufferSize_cb.connect([&](GLsizei width, GLsizei height) {
        window_size = Size2I{ width, height };
        Window::Current{ *window }.viewport(Vec2I{}, window_size);
    });

    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.2f, 0.3f, 0.3f, 1.0f });
    const auto window_process_input = [&current_window](bool is_esc_pressed) {
        if (is_esc_pressed) {
            current_window.set_should_close(true);
        }
    };

    auto prev_update_time = std::chrono::steady_clock::now();

    Projection<ProjectionType::Perspective> projection{
        .fov = glm::radians(45.0f),
        .near = 0.1f,
        .far = 100.0f,
    };

    const World world{};

    Camera camera{
        .tf =
            Transform{
                .tr = glm::vec3{ 0.0f, 0.0f, 3.0f },
                .rot = glm::angleAxis(glm::radians(-90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }),
            },
        .world_up = world.up,
    };

    Transform movement;
    const auto movement_process_input = [&movement](glm::vec3 world_direction, bool is_pressed) {
        if (is_pressed) {
            movement.translate(world_direction);
        }
    };

    (void)window->Key_cb.connect([&](int key, int scancode [[maybe_unused]], int action, int mods [[maybe_unused]]) {
        const bool is_pressed = (action & GLFW_PRESS) != 0;
        const bool is_released [[maybe_unused]] = (action & GLFW_RELEASE) != 0;
        const bool is_repeated [[maybe_unused]] = (action & GLFW_REPEAT) != 0;
        switch (key) {
        case GLFW_KEY_ESCAPE:
            window_process_input(is_pressed);
            break;
        }
    });

    const auto keyboard_process_input = [&](const Window::Current& cw) {
        if (cw.is_key_pressed(GLFW_KEY_W)) {
            movement_process_input(world.forward, true);
        }
        if (cw.is_key_pressed(GLFW_KEY_S)) {
            movement_process_input(-world.forward, true);
        }
        if (cw.is_key_pressed(GLFW_KEY_D)) {
            movement_process_input(world.right, true);
        }
        if (cw.is_key_pressed(GLFW_KEY_A)) {
            movement_process_input(-world.right, true);
        }
        if (cw.is_key_pressed(GLFW_KEY_Q)) {
            movement_process_input(world.up, true);
        }
        if (cw.is_key_pressed(GLFW_KEY_E)) {
            movement_process_input(-world.up, true);
        }
    };

    current_window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    std::optional<Vec2D> last_cursor_pos{};

    (void)window->CursorPos_cb.connect([&](double xpos, double ypos) {
        const Vec2D cursor_pos{ xpos, ypos };
        if (!last_cursor_pos.has_value()) {
            last_cursor_pos = cursor_pos;
            return;
        }
        const Vec2D cursor_offset{
            last_cursor_pos->x - cursor_pos.x,
            last_cursor_pos->y - cursor_pos.y,
        };
        last_cursor_pos = cursor_pos;

        constexpr float sensitivity = 0.1f;
        const float yaw = static_cast<float>(cursor_offset.x) * sensitivity;
        const float pitch = static_cast<float>(cursor_offset.y) * sensitivity;

        const auto camera_state = camera.state();
        const glm::quat rotation = glm::angleAxis(glm::radians(yaw), camera_state.up)
                                   * glm::angleAxis(glm::radians(pitch), camera_state.right);
        camera.tf.rotate(rotation);
    });

    const auto update = [&](float delta_time, const Transform& movement) {
        constexpr float camera_acc = 2.5f;
        const float camera_speed = camera_acc * delta_time;
        // TODO: movement inverted
        camera.tf.translate(camera_speed * (camera.tf.rot * movement.tr));
    };

    // prepare render
    const std::tuple texs{ create_texture("textures/cosmos.jpg"), create_texture("textures/osaka.jpg") };
    const std::array<std::string_view, 2> tex_uniform_names{ "us_texture_bg", "us_texture_fg" };
    const auto shader = create_shader(std::span{ tex_uniform_names });
    const auto buffers = create_buffers(std::span{ vertices_w_tex_coords });
    current_window.enable(GL_DEPTH_TEST);

    while (!current_window.should_close()) {
        // time
        const float delta_time = [&] {
            const auto curr_update_time = std::chrono::steady_clock::now();
            const auto delta_update_time = curr_update_time - std::exchange(prev_update_time, curr_update_time);
            return std::chrono::duration<float>(delta_update_time).count();
        }();

        // input
        const Transform curr_movement = [&] {
            movement = Transform{};
            ctx.poll_events();
            keyboard_process_input(current_window);
            return normalize(movement);
        }();

        // update
        {
            update(delta_time, curr_movement); //
        }

        // render
        {
            current_window.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const auto& [vb, eb, va] = buffers;
            const auto& [sp, set_transform] = shader;

            Draw draw(sp, va, texs);

            const glm::mat4 projection_matrix = projection.matrix(window_size);
            const glm::mat4 view_matrix = camera.state().view();

            for (const auto& [index, pos] : ranges::views::enumerate(cube_positions)) {
                const float angle = 20.0f * (static_cast<float>(index));
                const glm::mat4 model = glm::rotate( // 1st op
                    glm::translate(glm::mat4(1.0f), pos), // 2nd op
                    glm::radians(angle),
                    glm::vec3(1.0f, 0.3f, 0.5f)
                );
                const glm::mat4 transform = projection_matrix * view_matrix * model;
                set_transform(draw.sp_bind(), glm::value_ptr(transform));
                draw.elements(eb);
            }

            current_window.swap_buffers();
        }
    }

    return 0;
}
