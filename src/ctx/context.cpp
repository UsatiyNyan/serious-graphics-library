//
// Created by usatiynyan on 11/12/23.
//

#include "sl/gfx/ctx/context.hpp"
#include "sl/gfx/common/log.hpp"
#include "sl/gfx/common/vendors.hpp"

namespace sl::gfx {
namespace {
void apply_options(const Context::Options& options) {
    LOG_DEBUG("glfwInit version: {}.{} profile: {}", options.version_major, options.version_minor, options.profile);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, options.profile);
}
} // namespace

tl::optional<Context> Context::create(const Context::Options& options) {
    if (glfwInit() != GLFW_TRUE) {
        log::error("glfwInit");
        return tl::nullopt;
    }
    apply_options(options);
    return Context{};
}

void Context::poll_events() { glfwPollEvents(); } // NOLINT(*-convert-member-functions-to-static)

Context::Context()
    : finalizer{ [](Context&) {
          LOG_DEBUG("glfwTerminate");
          glfwTerminate();
      } } {}
} // namespace sl::gfx
