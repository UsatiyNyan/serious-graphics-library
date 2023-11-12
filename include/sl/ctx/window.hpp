//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/common/vendors.hpp"
#include "sl/primitives/color.hpp"
#include "sl/primitives/size.hpp"
#include "sl/primitives/vec.hpp"

#include "sl/ctx/context.hpp"

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

    private:
        Window* window_;
    };

private:
    explicit Window(GLFWwindow* glfw_window);

public:
    static std::unique_ptr<Window> create(const Context&, std::string_view title, Size2I size);
    ~Window() noexcept;

    Current make_current(Vec2I point, Size2I size, Color4F color);

private:
    GLFWwindow* glfw_window_;
};

} // namespace sl::gfx
