#pragma once

#include "base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Monado {

    class Log {
    public:
        static void Init();

        static Ref<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

    private:
        static Ref<spdlog::logger> s_CoreLogger;
        static Ref<spdlog::logger> s_ClientLogger;
    };

} // namespace Monado

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::vec<L, T, Q> &vector) {
    return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::mat<C, R, T, Q> &matrix) {
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, glm::qua<T, Q> quaternion) {
    return os << glm::to_string(quaternion);
}

// Core log macros
#define MND_CORE_TRACE(...) ::Monado::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MND_CORE_INFO(...) ::Monado::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MND_CORE_WARN(...) ::Monado::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MND_CORE_ERROR(...) ::Monado::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MND_CORE_CRITICAL(...) ::Monado::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MND_TRACE(...) ::Monado::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MND_INFO(...) ::Monado::Log::GetClientLogger()->info(__VA_ARGS__)
#define MND_WARN(...) ::Monado::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MND_ERROR(...) ::Monado::Log::GetClientLogger()->error(__VA_ARGS__)
#define MND_CRITICAL(...) ::Monado::Log::GetClientLogger()->critical(__VA_ARGS__)
