//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/primitives.hpp"

#include "sl/meta/conn/signal.hpp"

#include <memory>
#include <string_view>

namespace sl::gfx {

class Window {
public:
    class Current {
    public:
        explicit Current(Window& window);

        void enable(GLenum cap);
        int get_input_mode(int mode) const;
        void set_input_mode(int mode, int value);

        Vec2D get_cursor_pos() const;
        Vec2F get_content_scale() const;

        void viewport(Vec2I point, Size2I size);
        void set_clear_color(Color4F color);
        void clear(GLbitfield mask);
        void swap_buffers();

        bool should_close() const;
        void set_should_close(bool value);

        bool is_key_pressed(int key) const;

    private:
        Window* window_;
    };

public:
    static std::unique_ptr<Window> create(const Context&, std::string_view title, Size2I size);
    ~Window() noexcept;

    [[nodiscard]] Current make_current(Vec2I point, Size2I size, Color4F color);
    [[nodiscard]] GLFWwindow* glfw_window() const;

private:
    explicit Window(GLFWwindow* glfw_window);
    void setup_callbacks();

public:
    meta::signal<int, int> WindowPos_cb;
    meta::signal<int, int> WindowSize_cb;
    meta::signal<> WindowClose_cb;
    meta::signal<> WindowRefresh_cb;
    meta::signal<int> WindowFocus_cb;
    meta::signal<int> WindowIconify_cb;
    meta::signal<int> WindowMaximize_cb;
    meta::signal<GLsizei, GLsizei> FramebufferSize_cb;
    meta::signal<float, float> WindowContentScale_cb;
    meta::signal<int, int, int, int> Key_cb;
    meta::signal<unsigned int> Char_cb;
    meta::signal<unsigned int, int> CharMods_cb;
    meta::signal<int, int, int> MouseButton_cb;
    meta::signal<double, double> CursorPos_cb;
    meta::signal<unsigned int> CursorEnter_cb;
    meta::signal<double, double> Scroll_cb;
    meta::signal<int, const char**> Drop_cb;

private:
    GLFWwindow* glfw_window_;
};

} // namespace sl::gfx
