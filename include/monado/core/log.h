#pragma once

#include "core.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Monado {
    class Log {
    public:
        Log();
        ~Log();
        static void Init();
        static inline std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
        static inline std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
} // namespace Monado

// 这个用于 Client 的 Log
#define LOG(...) ::Monado::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARNING(...) ::Monado::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Monado::Log::GetClientLogger()->error(__VA_ARGS__)

// 这个用于实际整个工程的 CommandLine 的 Log
#define CORE_LOG(...) ::Monado::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARNING(...) ::Monado::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...) ::Monado::Log::GetCoreLogger()->error(__VA_ARGS__)
