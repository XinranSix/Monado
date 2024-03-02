#pragma once

#include <memory>

#ifdef MND_PLATFORM_WINDOWS
    #ifdef MND_BUILD_DLL
        #ifdef MND_DLL_EXPORT
            #define MND_API _declspec(dllexport)
        #else
            #define MND_API _declspec(dllimport)
        #endif
    #else
        #define MND_API
    #endif
#else
    #define MND_API
#endif

#ifdef MND_DEBUG
    #define MND_ENABLE_ASSERTS
#endif

#ifdef MND_ENABLE_ASSERTS
    #include "log.h"
    #define MND_ASSERT(x, ...)                                                                                         \
        if (!x) {                                                                                                      \
            MND_ERROR("Assertion Failed At: {0}", __VA_ARGS__);                                                        \
            __debugbreak();                                                                                            \
        }
    #define MND_CORE_ASSERT(x, ...)                                                                                    \
        if (!x) {                                                                                                      \
            MND_ERROR("Assertion Failed At: {0}", __VA_ARGS__);                                                        \
            __debugbreak();                                                                                            \
        }
#else
    #define MND_ASSERT(x, ...)
    #define MND_CORE_ASSERT(x, ...)
#endif

#define BIT(x) 1 << x

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

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
