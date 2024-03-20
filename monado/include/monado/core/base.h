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

	#define MND_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { MND_ERROR("Assertion Failed!"); __debugbreak(); } }
	#define MND_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { MND_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

	#define MND_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro

	#define MND_ASSERT(...) MND_ASSERT_RESOLVE(__VA_ARGS__, MND_ASSERT_MESSAGE, MND_ASSERT_NO_MESSAGE)(__VA_ARGS__)
	#define MND_CORE_ASSERT(...) MND_ASSERT_RESOLVE(__VA_ARGS__, MND_ASSERT_MESSAGE, MND_ASSERT_NO_MESSAGE)(__VA_ARGS__)
#else
	#define MND_ASSERT(...)
	#define MND_CORE_ASSERT(...)
#endif

#define BIT(x) (1 << x)

#define MND_BIND_EVENT_FN(fn)                                                                                          \
    [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Pointer wrappers
namespace Monado {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	using byte = unsigned char;
}
