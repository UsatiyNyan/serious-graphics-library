//
// Created by usatiynyan on 11/18/23.
//

#pragma once

#include "sl/gfx/common/log.hpp"

#include <function2/function2.hpp>

namespace sl {
// TODO(@usatiynyan): move to sl-meta?
// CRTP
template <typename Self>
class finalizer {
public:
    template <typename U, typename = std::enable_if_t<!std::is_same_v<finalizer, U>>> // to enable move operation
    explicit finalizer(U&& f) : f_{ std::forward<U>(f) } {}

    ~finalizer() noexcept {
        if (!f_) {
            return;
        }
        try {
            f_(*static_cast<Self*>(this));
        } catch (const std::exception& ex) {
            log::critical("finalizer: {}", ex.what());
        }
    }

    finalizer(const finalizer&) = delete;
    finalizer& operator=(const finalizer&) = delete;

    finalizer(finalizer&& other) noexcept { f_.swap(other.f_); }
    finalizer& operator=(finalizer&& other) noexcept {
        f_.swap(other.f_);
        return *this;
    }

private:
    fu2::unique_function<void(Self& self)> f_{};
};
} // namespace sl
