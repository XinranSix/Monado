#pragma once

#include <memory>

#ifdef MND_DEBUG
    #if defined(MND_PLATFORM_WINDOWS)
        #define MND_DEBUGBREAK() __debugbreak()
    #elif defined(MND_PLATFORM_LINUX)
        #include <signal.h>
        #define MND_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
    #define MND_ENABLE_ASSERTS
#else
    #define MND_DEBUGBREAK()
#endif

#define MND_EXPAND_MACRO(x) x
#define MND_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define MND_BIND_EVENT_FN(fn)                                                                                          \
    [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Monado {

    template <typename T>
    using Scope = std::unique_ptr<T>;
    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args &&...args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args &&...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace Monado

#include "log.h"
#include "assert.h"
