#pragma once

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
#define MONADO_ASSERT(x, ...)                                                                                          \
    if (!x) {                                                                                                          \
        LOG_ERROR("Assertion Failed At: {0}", __VA_ARGS__);                                                            \
        __debugbreak();                                                                                                \
    }
#define MONADO_CORE_ASSERT(x, ...)                                                                                     \
    if (!x) {                                                                                                          \
        CORE_LOG_ERROR("Assertion Failed At: {0}", __VA_ARGS__);                                                       \
        __debugbreak();                                                                                                \
    }
#else
#define MONADO_ASSERT(x, ...)
#define MONADO_CORE_ASSERT(x, ...)
#endif

#define BIT(x) 1 << x

// todo
#define MONADO_PROFILING
