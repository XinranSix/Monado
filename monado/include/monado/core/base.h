#pragma once

#include <memory>

namespace Monado {

    void InitializeCore();
    void ShutdownCore();

} // namespace Monado

#ifdef MND_DEBUG
    #define MND_ENABLE_ASSERTS
#endif

#ifdef MND_PLATFORM_WINDOWS
    #if MND_DYNAMIC_LINK
        #ifdef MND_BUILD_DLL
            #define MONADO_API __declspec(dllexport)
        #else
            #define MONADO_API __declspec(dllimport)
        #endif
    #else
        #define MONADO_API
    #endif
#else
    #error Hazel only supports Windows!
#endif

#ifdef MND_ENABLE_ASSERTS
    #define MND_ASSERT(x, ...)                                                                                         \
        {                                                                                                              \
            if (!(x)) {                                                                                                \
                MND_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                       \
                __debugbreak();                                                                                        \
            }                                                                                                          \
        }
    #define MND_CORE_ASSERT(x, ...)                                                                                    \
        {                                                                                                              \
            if (!(x)) {                                                                                                \
                MND_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                  \
                __debugbreak();                                                                                        \
            }                                                                                                          \
        }
#else
    #define MND_ASSERT(x, ...)
    #define MND_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define MND_BIND_EVENT_FN(fn)                                                                                          \
    [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
