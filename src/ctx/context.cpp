//
// Created by usatiynyan on 11/12/23.
//

#include "sl/ctx/context.hpp"
#include "sl/common/log.hpp"
#include "sl/common/vendors.hpp"

namespace sl::gfx {
namespace {
void apply_options(const Context::Options& options) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, options.profile);
    log::debug("glfwInit version: {}.{} profile: {}", options.version_major, options.version_minor, options.profile);
}
} // namespace

std::unique_ptr<Context> Context::create(Context::Options options) {
    if (glfwInit() != GLFW_TRUE) {
        log::error("glfwInit");
        return nullptr;
    }
    apply_options(options);
    return std::unique_ptr<Context>{new Context{}};
}

void Context::poll_events() { glfwPollEvents(); } // NOLINT(*-convert-member-functions-to-static)

Context::~Context() noexcept { glfwTerminate(); }
} // namespace sl::gfx
