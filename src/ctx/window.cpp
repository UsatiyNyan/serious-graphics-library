//
// Created by usatiynyan on 11/12/23.
//

#include "sl/gfx/ctx/window.hpp"

#include "sl/gfx/common/log.hpp"

#include <libassert/assert.hpp>

namespace sl::gfx {
namespace current {
thread_local Window* window_;

Window* get() { return window_; }

void set(Window* window) { window_ = window; }

void load_gl() {
    static std::once_flag gl_loaded_;

    std::call_once(gl_loaded_, [] {
        ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0);
    });
}
} // namespace current

Window::Current::Current(Window& window) : window_{ &window } {
    if (current::get() != window_) {
        current::set(window_);
        glfwMakeContextCurrent(window_->glfw_window_);
        current::load_gl();
    }
}

void Window::Current::enable(GLenum cap) {
    ASSERT(current::get() == window_);
    glEnable(cap);
}

void Window::Current::set_input_mode(int mode, int value) {
    ASSERT(current::get() == window_);
    glfwSetInputMode(window_->glfw_window_, mode, value);
}

Vec2D Window::Current::get_cursor_pos() const {
    ASSERT(current::get() == window_);
    Vec2D cursor_pos;
    glfwGetCursorPos(window_->glfw_window_, &cursor_pos.x, &cursor_pos.y);
    return cursor_pos;
}

void Window::Current::viewport(Vec2I point, Size2I size) {
    ASSERT(current::get() == window_);
    glViewport(point.x, point.y, size.width, size.height);
}

void Window::Current::set_clear_color(Color4F color) {
    ASSERT(current::get() == window_);
    glClearColor(color.red, color.green, color.blue, color.alpha);
}

void Window::Current::clear(GLbitfield mask) {
    ASSERT(current::get() == window_);
    glClear(mask);
}

void Window::Current::swap_buffers() { glfwSwapBuffers(window_->glfw_window_); }

bool Window::Current::is_key_pressed(int key) const {
    ASSERT(current::get() == window_);
    // GLFW_RELEASE = 0
    // GLFW_PRESS = 1
    return glfwGetKey(window_->glfw_window_, key) == GLFW_PRESS;
}

bool Window::Current::should_close() const { return glfwWindowShouldClose(window_->glfw_window_); }

void Window::Current::set_should_close(bool value) { glfwSetWindowShouldClose(window_->glfw_window_, value); }

std::unique_ptr<Window> Window::create(const Context&, std::string_view title, Size2I size) {
    LOG_DEBUG("glfwCreateWindow \"{}\" ({}x{})", title, size.width, size.height);
    GLFWwindow* const glfw_window = glfwCreateWindow(size.width, size.height, title.data(), nullptr, nullptr);
    if (glfw_window == nullptr) {
        log::error("glfwCreateWindow");
        return nullptr;
    }

    return std::unique_ptr<Window>{ new Window{ glfw_window } };
}

Window::~Window() noexcept {
    if (current::get() == this) {
        current::set(nullptr);
    }
    glfwDestroyWindow(glfw_window_);
}

Window::Current Window::make_current(Vec2I point, Size2I size, Color4F color) {
    Current current_window{ *this };
    current_window.viewport(point, size);
    current_window.set_clear_color(color);
    return current_window;
}

GLFWwindow* Window::glfw_window() const { return glfw_window_; }

Window::Window(GLFWwindow* glfw_window) : glfw_window_{ glfw_window } { setup_callbacks(); }

void Window::setup_callbacks() {
    // ew macros-es

#define SETUP_WINDOW_CALLBACK(name)                                       \
    glfwSet##name##Callback(glfw_window_, [](GLFWwindow*, auto... args) { \
        Window* window = current::get();                                  \
        if (window != nullptr) {                                          \
            window->name##_cb(args...);                                   \
        }                                                                 \
    })

    SETUP_WINDOW_CALLBACK(WindowPos);
    SETUP_WINDOW_CALLBACK(WindowSize);
    SETUP_WINDOW_CALLBACK(WindowClose);
    SETUP_WINDOW_CALLBACK(WindowRefresh);
    SETUP_WINDOW_CALLBACK(WindowFocus);
    SETUP_WINDOW_CALLBACK(WindowIconify);
    SETUP_WINDOW_CALLBACK(WindowMaximize);
    SETUP_WINDOW_CALLBACK(FramebufferSize);
    SETUP_WINDOW_CALLBACK(WindowContentScale);
    SETUP_WINDOW_CALLBACK(Key);
    SETUP_WINDOW_CALLBACK(Char);
    SETUP_WINDOW_CALLBACK(CharMods);
    SETUP_WINDOW_CALLBACK(MouseButton);
    SETUP_WINDOW_CALLBACK(CursorPos);
    SETUP_WINDOW_CALLBACK(CursorEnter);
    SETUP_WINDOW_CALLBACK(Scroll);
    SETUP_WINDOW_CALLBACK(Drop);

#undef SETUP_WINDOW_CALLBACK
}

} // namespace sl::gfx
