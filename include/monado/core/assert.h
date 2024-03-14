#pragma once

#include "base.h"
#include "log.h"
#include <filesystem>

#ifdef MND_ENABLE_ASSERTS

    // Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
    // provide support for custom formatting by concatenating the formatting string instead of having the format inside
    // the default message
    #define MND_INTERNAL_ASSERT_IMPL(type, check, msg, ...)                                                            \
        {                                                                                                              \
            if (!(check)) {                                                                                            \
                MND##type##ERROR(msg, __VA_ARGS__);                                                                    \
                MND_DEBUGBREAK();                                                                                      \
            }                                                                                                          \
        }
    #define MND_INTERNAL_ASSERT_WITH_MSG(type, check, ...)                                                             \
        MND_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    #define MND_INTERNAL_ASSERT_NO_MSG(type, check)                                                                    \
        MND_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", MND_STRINGIFY_MACRO(check),         \
                                 std::filesystem::path(__FILE__).filename().string(), __LINE__)

    #define MND_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define MND_INTERNAL_ASSERT_GET_MACRO(...)                                                                         \
        MND_EXPAND_MACRO(                                                                                              \
            MND_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, MND_INTERNAL_ASSERT_WITH_MSG, MND_INTERNAL_ASSERT_NO_MSG))

    // Currently accepts at least the condition and one additional parameter (the message) being optional
    #define MND_ASSERT(...) MND_EXPAND_MACRO(MND_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
    #define MND_CORE_ASSERT(...) MND_EXPAND_MACRO(MND_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
    #define MND_ASSERT(...)
    #define MND_CORE_ASSERT(...)
#endif
