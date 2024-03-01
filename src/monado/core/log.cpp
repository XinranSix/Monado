#include "monado/core/log.h"

// #include "hzpch.h"

namespace Monado {
    Log::Log() {}

    Log::~Log() {}

    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        s_CoreLogger = spdlog::stdout_color_mt("Monado"); // mt means multi threaded
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stdout_color_mt("Console");
        s_ClientLogger->set_level(spdlog::level::trace);
    }

    std::shared_ptr<spdlog::logger> Log::s_ClientLogger { nullptr };
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger { nullptr };

} // namespace Monado
