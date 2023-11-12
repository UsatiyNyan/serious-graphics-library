//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include <memory>

namespace sl::gfx {

class Context {
public:
    struct Options {
        int version_major;
        int version_minor;
        int profile;
    };

    static std::unique_ptr<Context> create(Options options);
    void poll_events(); // TODO(@usatiynyan): return the events

private:
    Context() = default;

public:
    ~Context() noexcept;

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    Context(Context&&) noexcept = delete;
    Context& operator=(Context&&) noexcept = delete;
};

} // namespace sl::gfx
