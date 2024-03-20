#pragma once

#include "base.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Monado {

    class Log {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

} // namespace Monado

// Core Logging Macros
#define MND_CORE_TRACE(...) Monado::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MND_CORE_INFO(...) Monado::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MND_CORE_WARN(...) Monado::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MND_CORE_ERROR(...) Monado::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MND_CORE_FATAL(...) Monado::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Logging Macros
#define MND_TRACE(...) Monado::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MND_INFO(...) Monado::Log::GetClientLogger()->info(__VA_ARGS__)
#define MND_WARN(...) Monado::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MND_ERROR(...) Monado::Log::GetClientLogger()->error(__VA_ARGS__)
#define MND_FATAL(...) Monado::Log::GetClientLogger()->critical(__VA_ARGS__)
