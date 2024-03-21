#pragma once

#ifdef MND_DEBUG
	#define MND_ENABLE_ASSERTS
#endif

#ifdef MND_ENABLE_ASSERTS
	#define MND_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { MND_ERROR("Assertion Failed"); __debugbreak(); } }
	#define MND_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { MND_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

	#define MND_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
	#define MND_GET_ASSERT_MACRO(...) MND_EXPAND_VARGS(MND_ASSERT_RESOLVE(__VA_ARGS__, MND_ASSERT_MESSAGE, MND_ASSERT_NO_MESSAGE))

	#define MND_ASSERT(...) MND_EXPAND_VARGS( MND_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
	#define MND_CORE_ASSERT(...) MND_EXPAND_VARGS( MND_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
	#define MND_ASSERT(...)
	#define MND_CORE_ASSERT(...)
#endif
