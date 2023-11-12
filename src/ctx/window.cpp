//
// Created by usatiynyan on 11/12/23.
//

#include "sl/ctx/window.hpp"
#include "sl/common/log.hpp"

#include <assert.hpp>

namespace sl::gfx {
namespace current {
thread_local Window* window_;

Window* get() { return window_; }

void set(Window* window) { window_ = window; }

void load_gl() {
    static std::once_flag gl_loaded_;

    std::call_once(gl_loaded_, [] {
        const int result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        ASSERT(result != 0);
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

Window::Window(GLFWwindow* glfw_window) : glfw_window_{ glfw_window } {}

std::unique_ptr<Window> Window::create(const Context&, std::string_view title, Size2I size) {
    log::debug("glfwCreateWindow \"{}\" ({}x{})", title, size.width, size.height);
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
} // namespace sl::gfx
