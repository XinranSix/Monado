#pragma once

#include <memory>

#ifdef MND_PLATFORM_WINDOWS
#ifdef MND_BUILD_DLL
#define MONADO_API //_declspec (dllexport)
// #define IMGUI_API _declspec (dllexport)
#else
#define MONADO_API //_declspec (dllimport)
#define IMGUI_API  //_declspec (dllimport)
#endif             // HZ_BUILD_DLL
#endif

#ifdef MND_ENABLE_ASSERTS
#include "log.h"
#define MONADO_ASSERT(x, ...)                                                                                          \
    if (!x) {                                                                                                          \
        MONADO_ERROR("Assertion Failed At: {0}", __VA_ARGS__);                                                            \
        __debugbreak();                                                                                                \
    }
#define MONADO_CORE_ASSERT(x, ...)                                                                                     \
    if (!x) {                                                                                                          \
        MONADO_ERROR("Assertion Failed At: {0}", __VA_ARGS__);                                                       \
        __debugbreak();                                                                                                \
    }
#else
#define MONADO_ASSERT(x, ...)
#define MONADO_CORE_ASSERT(x, ...)
#endif

#define BIT(x) 1 << x

// todo
// #define MONADO_PROFILING

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
}; // namespace Monado
