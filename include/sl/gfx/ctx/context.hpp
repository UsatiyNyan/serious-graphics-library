//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/meta/lifetime/finalizer.hpp"

#include <atomic>
#include <memory>

namespace sl::gfx {

class context : public meta::finalizer<context> {
public:
    struct options {
        int version_major;
        int version_minor;
        int profile;
    };

public:
    static std::unique_ptr<context> create(const options& options);

    void load_gl();

    void poll_events(); // TODO(@usatiynyan): return the events

private:
    context();

private:
    std::atomic<bool> gl_loaded_{ false };
};

} // namespace sl::gfx
