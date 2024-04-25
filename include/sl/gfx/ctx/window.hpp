//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/primitives.hpp"

#include <sl/meta/conn/signal.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <span>
#include <string_view>

namespace sl::gfx {

class current_window;

class window {
public:
    static std::unique_ptr<window> create(const context&, std::string_view title, glm::ivec2 size);
    ~window() noexcept;

    [[nodiscard]] current_window make_current(context& context, glm::ivec2 point, glm::ivec2 size, glm::fvec4 color);
    [[nodiscard]] GLFWwindow* internal() const { return internal_; }

private:
    explicit window(GLFWwindow* internal);
    void setup_callbacks();

public:
    meta::signal<glm::ivec2> window_pos_cb;
    meta::signal<glm::ivec2> window_size_cb;
    meta::signal<> window_close_cb;
    meta::signal<> window_refresh_cb;
    meta::signal<bool> window_focus_cb;
    meta::signal<bool> window_iconify_cb;
    meta::signal<bool> window_maximize_cb;
    meta::signal<glm::fvec2> window_content_scale_cb;
    meta::signal<glm::ivec2> frame_buffer_size_cb;
    meta::signal<int, int, int, int> key_cb;
    meta::signal<unsigned int> char_cb;
    meta::signal<unsigned int, int> charmods_cb;
    meta::signal<int, int, int> mousebutton_cb;
    meta::signal<glm::dvec2> cursor_pos_cb;
    meta::signal<bool> cursor_enter_cb;
    meta::signal<glm::dvec2> scroll_cb;
    meta::signal<std::span<const char*>> drop_cb;

private:
    GLFWwindow* internal_;
};

class current_window {
public:
    explicit current_window(window& window);

    void enable(GLenum cap);
    void viewport(glm::ivec2 point, glm::ivec2 size);
    void set_clear_color(glm::fvec4 color);
    void clear(GLbitfield mask);
    void swap_buffers();

    bool should_close() const;
    void set_should_close(bool value);

    int get_input_mode(int mode) const;
    void set_input_mode(int mode, int value);

    glm::dvec2 get_cursor_pos() const;
    glm::fvec2 get_content_scale() const;

    bool is_key_pressed(int key) const;

private:
    window* window_;
};

template <typename T = float>
    requires std::is_arithmetic_v<T>
T aspect_ratio(glm::ivec2 window_size) {
    const int width = window_size.x;
    const int height = window_size.y;
    return static_cast<T>(width) / static_cast<T>(height);
}

} // namespace sl::gfx
