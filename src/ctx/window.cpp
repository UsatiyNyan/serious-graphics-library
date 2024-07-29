//
// Created by usatiynyan on 11/12/23.
//

#include "sl/gfx/ctx/window.hpp"

#include "sl/gfx/detail/log.hpp"

#include <GLFW/glfw3.h>
#include <libassert/assert.hpp>

namespace sl::gfx {
namespace this_thread {

thread_local window* window_;

window* get_window() { return window_; }

void set_window(window* window) { window_ = window; }

} // namespace this_thread

std::unique_ptr<window> window::create(const context&, std::string_view title, glm::ivec2 size) {
    log::trace("glfwCreateWindow \"{}\" ({}x{})", title, size.x, size.y);
    GLFWwindow* const internal = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
    if (internal == nullptr) {
        log::error("glfwCreateWindow");
        return nullptr;
    }
    return std::unique_ptr<window>{ new window{ internal } };
}

window::~window() noexcept {
    if (this_thread::get_window() == this) {
        this_thread::set_window(nullptr);
    }
    glfwDestroyWindow(internal_);
}

current_window window::make_current(context& context, glm::ivec2 point, glm::ivec2 size, glm::fvec4 color) {
    const bool context_needs_load = this_thread::get_window() != this;
    current_window current_window{ *this };
    if (context_needs_load) {
        context.load_gl();
    }
    current_window.viewport(point, size);
    current_window.set_clear_color(color);
    return current_window;
}

window::window(GLFWwindow* internal) : internal_{ internal } { setup_callbacks(); }

void window::setup_callbacks() {
    glfwSetWindowPosCallback(internal_, [](GLFWwindow*, int xpos, int ypos) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_pos_cb(glm::ivec2{ xpos, ypos });
        }
    });
    glfwSetWindowSizeCallback(internal_, [](GLFWwindow*, int width, int height) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_size_cb(glm::ivec2{ width, height });
        }
    });
    glfwSetWindowCloseCallback(internal_, [](GLFWwindow*) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_close_cb();
        }
    });
    glfwSetWindowRefreshCallback(internal_, [](GLFWwindow*) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_refresh_cb();
        }
    });
    glfwSetWindowFocusCallback(internal_, [](GLFWwindow*, int focused) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_focus_cb(focused == GLFW_TRUE);
        }
    });
    glfwSetWindowIconifyCallback(internal_, [](GLFWwindow*, int iconified) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_iconify_cb(iconified == GLFW_TRUE);
        }
    });
    glfwSetWindowMaximizeCallback(internal_, [](GLFWwindow*, int maximized) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_maximize_cb(maximized == GLFW_TRUE);
        }
    });
    glfwSetWindowContentScaleCallback(internal_, [](GLFWwindow*, float xscale, float yscale) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->window_content_scale_cb(glm::fvec2{ xscale, yscale });
        }
    });
    glfwSetFramebufferSizeCallback(internal_, [](GLFWwindow*, int width, int height) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->frame_buffer_size_cb(glm::ivec2{ width, height });
        }
    });
    glfwSetKeyCallback(internal_, [](GLFWwindow*, int key, int scancode, int action, int mods) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->key_cb(key, scancode, action, mods);
        }
    });
    glfwSetCharCallback(internal_, [](GLFWwindow*, unsigned int codepoint) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->char_cb(codepoint);
        }
    });
    glfwSetMouseButtonCallback(internal_, [](GLFWwindow*, int button, int action, int mods) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->mouse_button_cb(button, action, mods);
        }
    });
    glfwSetCursorPosCallback(internal_, [](GLFWwindow*, double xpos, double ypos) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->cursor_pos_cb(glm::dvec2{ xpos, ypos });
        }
    });
    glfwSetCursorEnterCallback(internal_, [](GLFWwindow*, int entered) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->cursor_enter_cb(entered == GLFW_TRUE);
        }
    });
    glfwSetScrollCallback(internal_, [](GLFWwindow*, double xoffset, double yoffset) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->scroll_cb(glm::dvec2{ xoffset, yoffset });
        }
    });
    glfwSetDropCallback(internal_, [](GLFWwindow*, int path_count, const char* paths[]) {
        if (window* window = this_thread::get_window(); window != nullptr) {
            window->drop_cb(std::span<const char*>{ paths, static_cast<std::size_t>(path_count) });
        }
    });
}

current_window::current_window(window& window) : window_{ &window } {
    if (this_thread::get_window() != window_) {
        this_thread::set_window(window_);
        glfwMakeContextCurrent(window_->internal());
    }
}

void current_window::enable(GLenum cap) {
    ASSERT(this_thread::get_window() == window_);
    glEnable(cap);
}

void current_window::viewport(glm::ivec2 point, glm::ivec2 size) {
    ASSERT(this_thread::get_window() == window_);
    glViewport(point.x, point.y, size.x, size.y);
}

void current_window::set_clear_color(glm::fvec4 color) {
    ASSERT(this_thread::get_window() == window_);
    glClearColor(color.r, color.g, color.b, color.a);
}

void current_window::clear(GLbitfield mask) {
    ASSERT(this_thread::get_window() == window_);
    glClear(mask);
}

void current_window::swap_buffers() { glfwSwapBuffers(window_->internal()); }

bool current_window::should_close() const { return glfwWindowShouldClose(window_->internal()); }

void current_window::set_should_close(bool value) { glfwSetWindowShouldClose(window_->internal(), value); }

int current_window::get_input_mode(int mode) const {
    ASSERT(this_thread::get_window() == window_);
    return glfwGetInputMode(window_->internal(), mode);
}

void current_window::set_input_mode(int mode, int value) {
    ASSERT(this_thread::get_window() == window_);
    glfwSetInputMode(window_->internal(), mode, value);
}

glm::dvec2 current_window::get_cursor_pos() const {
    ASSERT(this_thread::get_window() == window_);
    glm::dvec2 cursor_pos;
    glfwGetCursorPos(window_->internal(), &cursor_pos.x, &cursor_pos.y);
    return cursor_pos;
}

glm::fvec2 current_window::get_content_scale() const {
    ASSERT(this_thread::get_window() == window_);
    glm::fvec2 content_scale;
    glfwGetWindowContentScale(window_->internal(), &content_scale.x, &content_scale.y);
    return content_scale;
}

bool current_window::is_key_pressed(int key) const {
    ASSERT(this_thread::get_window() == window_);
    // GLFW_RELEASE = 0
    // GLFW_PRESS = 1
    return glfwGetKey(window_->internal(), key) == GLFW_PRESS;
}

} // namespace sl::gfx
