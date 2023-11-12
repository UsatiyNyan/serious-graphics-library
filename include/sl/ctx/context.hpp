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

private:
    Context() = default;

public:
    static std::unique_ptr<Context> create(Options options);
    ~Context() noexcept;

    void poll_events(); // TODO(@usatiynyan): return the events
};

} // namespace sl::gfx
