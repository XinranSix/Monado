#pragma once

#include <memory>

namespace Monado {

    void InitializeCore();
    void ShutdownCore();

} // namespace Monado

#ifndef MND_PLATFORM_WINDOWS
    #error Monado only supports Windows!
#endif

// __VA_ARGS__ expansion to get past MSVC "bug"
#define MND_EXPAND_VARGS(x) x

#define BIT(x) (1 << x)

#define MND_BIND_EVENT_FN(fn)                                                                                          \
    [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "assert.h"

// Pointer wrappers
namespace Monado {

    template <typename T>
    using Scope = std::unique_ptr<T>;

    template <typename T>
    using Ref = std::shared_ptr<T>;

    using byte = unsigned char;

} // namespace Monado
