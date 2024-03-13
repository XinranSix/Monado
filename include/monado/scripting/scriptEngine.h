#pragma once

namespace Monado {

    class ScriptEngine {
    public:
        static void Init();
        static void Shutdown();

    private:
        static void InitMono();
        static void ShutdownMono();
    };

} // namespace Monado
