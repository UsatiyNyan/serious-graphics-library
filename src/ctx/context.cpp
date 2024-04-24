//
// Created by usatiynyan on 11/12/23.
//

#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/common/log.hpp"
#include "sl/gfx/common/vendors.hpp"

#include <libassert/assert.hpp>
#include <tl/optional.hpp>

namespace sl::gfx {
namespace {

void apply_options(const context::options& options) {
    LOG_DEBUG("glfwInit version: {}.{} profile: {}", options.version_major, options.version_minor, options.profile);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, options.profile);
}

} // namespace

std::unique_ptr<context> context::create(const context::options& options) {
    if (glfwInit() != GLFW_TRUE) {
        log::error("glfwInit");
        return nullptr;
    }
    apply_options(options);
    return std::unique_ptr<context>{ new context };
}

void context::load_gl() {
    if (!gl_loaded_.exchange(true)) {
        ASSERT(gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0);
    }
}

void context::poll_events() { glfwPollEvents(); } // NOLINT(*-convert-member-functions-to-static)

context::context()
    : finalizer{ [](context&) {
          LOG_DEBUG("glfwTerminate");
          glfwTerminate();
      } } {}

} // namespace sl::gfx
