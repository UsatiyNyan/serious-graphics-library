//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/gfx/common/finalizer.hpp"
#include "sl/gfx/common/vendors.hpp"
#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/primitives.hpp"

#include <function2/function2.hpp>

#include <memory>
#include <string_view>

namespace sl::gfx {

class Window {
public:
    class Current {
    public:
        explicit Current(Window& window);

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

    Current make_current(Vec2I point, Size2I size, Color4F color);

private:
    explicit Window(GLFWwindow* glfw_window) : glfw_window_{ glfw_window } {}
    void setup_callbacks();

public:
    fu2::unique_function<void(int, int)> WindowPos_cb;
    fu2::unique_function<void(int, int)> WindowSize_cb;
    fu2::unique_function<void()> WindowClose_cb;
    fu2::unique_function<void()> WindowRefresh_cb;
    fu2::unique_function<void(int)> WindowFocus_cb;
    fu2::unique_function<void(int)> WindowIconify_cb;
    fu2::unique_function<void(int)> WindowMaximize_cb;
    fu2::unique_function<void(GLsizei, GLsizei)> FramebufferSize_cb;
    fu2::unique_function<void(float, float)> WindowContentScale_cb;
    fu2::unique_function<void(int, int, int, int)> Key_cb;
    fu2::unique_function<void(unsigned int)> Char_cb;
    fu2::unique_function<void(unsigned int, int)> CharMods_cb;
    fu2::unique_function<void(int, int, int)> MouseButton_cb;
    fu2::unique_function<void(double, double)> CursorPos_cb;
    fu2::unique_function<void(unsigned int)> CursorEnter_cb;
    fu2::unique_function<void(unsigned int, int)> Scroll_cb;
    fu2::unique_function<void(int, const char*[])> Drop_cb;

private:
    GLFWwindow* glfw_window_;
};

} // namespace sl::gfx
