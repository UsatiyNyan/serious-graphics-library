//
// Created by usatiynyan on 11/12/23.
//

#pragma once

#include "sl/meta/lifetime/finalizer.hpp"

#include <tl/optional.hpp>

namespace sl::gfx {

class Context: public finalizer<Context> {
public:
    struct Options {
        int version_major;
        int version_minor;
        int profile;
    };

public:
    static tl::optional<Context> create(const Options& options);

    void poll_events(); // TODO(@usatiynyan): return the events

private:
    Context();
};

} // namespace sl::gfx
