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
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr std::array cube_vertices{
    // positions
    // front face
    VT{ 0.5f, 0.5f, 0.5f }, // top right
    VT{ 0.5f, -0.5f, 0.5f }, // bottom right
    VT{ -0.5f, -0.5f, 0.5f }, // bottom left
    VT{ -0.5f, 0.5f, 0.5f }, // top left
    // right face
    VT{ 0.5f, 0.5f, 0.5f }, // top right
    VT{ 0.5f, -0.5f, 0.5f }, // bottom right
    VT{ 0.5f, -0.5f, -0.5f }, // bottom left
    VT{ 0.5f, 0.5f, -0.5f }, // top left
    // back face
    VT{ 0.5f, 0.5f, -0.5f }, // top right
    VT{ 0.5f, -0.5f, -0.5f }, // bottom right
    VT{ -0.5f, -0.5f, -0.5f }, // bottom left
    VT{ -0.5f, 0.5f, -0.5f }, // top left
    // left face
    VT{ -0.5f, 0.5f, -0.5f }, // top right
    VT{ -0.5f, -0.5f, -0.5f }, // bottom right
    VT{ -0.5f, -0.5f, 0.5f }, // bottom left
    VT{ -0.5f, 0.5f, 0.5f }, // top left
    // top face
    VT{ 0.5f, 0.5f, 0.5f }, // top right
    VT{ 0.5f, 0.5f, -0.5f }, // bottom right
    VT{ -0.5f, 0.5f, -0.5f }, // bottom left
    VT{ -0.5f, 0.5f, 0.5f }, // top left
    // bottom face
    VT{ 0.5f, -0.5f, 0.5f }, // top right
    VT{ 0.5f, -0.5f, -0.5f }, // bottom right
    VT{ -0.5f, -0.5f, -0.5f }, // bottom left
    VT{ -0.5f, -0.5f, 0.5f }, // top left
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
};

constexpr std::array source_positions{
    glm::vec3{ 2.0f, 2.0f, -3.0f }, //
};

auto create_object_shader() {
    std::array<const Shader, 2> shaders{
        *ASSERT_VAL(Shader::load_from_file(ShaderType::VERTEX, "shaders/13_lighting_object.vert")),
        *ASSERT_VAL(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/13_lighting_object.frag")),
    };
    auto sp = *ASSERT_VAL(ShaderProgram::build(std::span{ shaders }));
    auto sp_bind = sp.bind();
    auto set_transform = *ASSERT_VAL(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
    auto set_light_color = *ASSERT_VAL(sp_bind.make_uniform_setter(glUniform3f, "u_light_color"));
    auto set_object_color = *ASSERT_VAL(sp_bind.make_uniform_setter(glUniform3f, "u_object_color"));
    return std::make_tuple(
        std::move(sp), std::move(set_transform), std::move(set_light_color), std::move(set_object_color)
    );
};

auto create_source_shader() {
    std::array<const Shader, 2> shaders{
        *ASSERT_VAL(Shader::load_from_file(ShaderType::VERTEX, "shaders/13_lighting_source.vert")),
        *ASSERT_VAL(Shader::load_from_file(ShaderType::FRAGMENT, "shaders/13_lighting_source.frag")),
    };
    auto sp = *ASSERT_VAL(ShaderProgram::build(std::span{ shaders }));
    auto sp_bind = sp.bind();
    auto set_transform = *ASSERT_VAL(sp_bind.make_uniform_matrix_v_setter(glUniformMatrix4fv, "u_transform", 1, false));
    auto set_light_color = *ASSERT_VAL(sp_bind.make_uniform_setter(glUniform3f, "u_light_color"));
    return std::make_tuple(std::move(sp), std::move(set_transform), std::move(set_light_color));
};

using buffers_type = std::tuple<
    Buffer<VT, BufferType::ARRAY, BufferUsage::STATIC_DRAW>,
    Buffer<unsigned, BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW>,
    VertexArray>;

template <std::size_t extent>
buffers_type create_buffers(std::span<const VT, extent> vt_coords) {
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
    const auto window = ASSERT_VAL(Window::create(ctx, "13_lighting_color", window_size));
    (void)window->FramebufferSize_cb.connect([&](GLsizei width, GLsizei height) {
        window_size = Size2I{ width, height };
        Window::Current{ *window }.viewport(Vec2I{}, window_size);
    });

    auto current_window = window->make_current(Vec2I{}, window_size, Color4F{ 0.1f, 0.1f, 0.1f, 1.0f });

    auto prev_update_time = std::chrono::steady_clock::now();

    constexpr Basis world{};

    Camera camera{
        .tf =
            Transform{
                .tr = glm::vec3{ 0.0f, 0.0f, 3.0f },
                .rot = glm::angleAxis(glm::radians(-180.0f), world.up()),
            },
        .proj =
            PerspectiveProjection{
                .fov = glm::radians(45.0f),
                .near = 0.1f,
                .far = 100.0f,
            },
    };

    const auto transform_from_keyboard = [&world](const Window::Current& cw) {
        Transform movement{};
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
            movement.translate(-world.up());
        }
        if (cw.is_key_pressed(GLFW_KEY_E)) {
            movement.translate(world.up());
        }
        return normalize(movement);
    };

    current_window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    std::optional<glm::vec2> last_cursor_pos{};

    (void)window->CursorPos_cb.connect([&](double xpos, double ypos) {
        const glm::vec2 cursor_pos{ xpos, ypos };
        if (!last_cursor_pos.has_value()) {
            last_cursor_pos = cursor_pos;
            return;
        }
        const glm::vec2 cursor_offset = *last_cursor_pos - cursor_pos;
        last_cursor_pos = cursor_pos;

        constexpr float sensitivity = glm::radians(0.1f);
        const float yaw = cursor_offset.x * sensitivity;
        const float pitch = cursor_offset.y * sensitivity;

        const glm::quat rot_yaw = glm::angleAxis(yaw, world.y);
        camera.tf.rotate(rot_yaw);

        const glm::vec3 camera_forward = camera.tf.rot * world.forward();
        const glm::vec3 camera_right = glm::cross(camera_forward, world.up());
        const glm::quat rot_pitch = glm::angleAxis(pitch, camera_right);
        camera.tf.rotate(rot_pitch);
    });

    const auto update = [&](float delta_time, const Transform& movement) {
        constexpr float camera_acc = 2.5f;
        const float camera_speed = camera_acc * delta_time;
        camera.tf.translate(camera_speed * (camera.tf.rot * movement.tr));
    };

    // prepare render
    const auto object_shader = create_object_shader();
    const auto object_buffers = create_buffers(std::span{ cube_vertices });

    const auto source_shader = create_source_shader();
    const auto source_buffers = create_buffers(std::span{ cube_vertices });
    const glm::vec3 source_color{ 1.0f, 1.0f, 1.0f };

    current_window.enable(GL_DEPTH_TEST);

    // main loop
    while (!current_window.should_close()) {
        // input
        ctx.poll_events();

        if (current_window.is_key_pressed(GLFW_KEY_ESCAPE)) {
            current_window.set_should_close(true);
        }

        const Transform movement = transform_from_keyboard(current_window);

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
        current_window.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 projection = camera.projection(window_size);
        const glm::mat4 view = camera.view(world);

        // objects
        {
            const auto& [vb, eb, va] = object_buffers;
            const auto& [sp, set_transform, set_light_color, set_object_color] = object_shader;

            Draw draw(sp, va, {});

            set_light_color(draw.sp_bind(), source_color.r, source_color.g, source_color.b);
            set_object_color(draw.sp_bind(), 0.61f, 0.08f, 0.90f); // #9c15e6

            for (const auto& pos : cube_positions) {
                const glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                const glm::mat4 transform = projection * view * model;
                set_transform(draw.sp_bind(), glm::value_ptr(transform));
                draw.elements(eb);
            }
        }

        // source
        {
            const auto& [vb, eb, va] = source_buffers;
            const auto& [sp, set_transform, set_light_color] = source_shader;

            Draw draw(sp, va, {});

            set_light_color(draw.sp_bind(), source_color.r, source_color.g, source_color.b);

            for (const auto& pos : source_positions) {
                const glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                const glm::mat4 transform = projection * view * model;
                set_transform(draw.sp_bind(), glm::value_ptr(transform));
                draw.elements(eb);
            }
        }

        current_window.swap_buffers();
    }

    return 0;
}
