#include "monado/core/log.h"

#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <filesystem>

#include <memory>

namespace Monado {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // Create "logs" directory if doesn't exist
        std::string logsDirectory = "logs";
        if (!std::filesystem::exists(logsDirectory))
            std::filesystem::create_directories(logsDirectory);

        spdlog::set_pattern("%^[%T] %n: %v%$");

        std::vector<spdlog::sink_ptr> monadoSinks = { std::make_shared<spdlog::sinks::stdout_sink_mt>(),
                                                      std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                                                          "logs/MONADO.log", true) };

        std::vector<spdlog::sink_ptr> appSinks = { std::make_shared<spdlog::sinks::stdout_sink_mt>(),
                                                   std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/APP.log",
                                                                                                       true) };

        s_CoreLogger = std::make_shared<spdlog::logger>("MONADO", monadoSinks.begin(), monadoSinks.end());
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = std::make_shared<spdlog::logger>("APP", appSinks.begin(), appSinks.end());
        s_ClientLogger->set_level(spdlog::level::trace);
    }

    void Log::Shutdown() {
        s_ClientLogger.reset();
        s_CoreLogger.reset();
        spdlog::drop_all();
    }

} // namespace Monado
