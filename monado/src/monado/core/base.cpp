#include "monado/core/base.h"
#include "monado/core/log.h"

#define MONADO_BUILD_ID "v0.1a"

namespace Monado {

    void InitializeCore() {
        Log::Init();

        MND_CORE_TRACE("Hazel Engine {}", MONADO_BUILD_ID);
        MND_CORE_TRACE("Initializing...");
    }

    void ShutdownCore() { MND_CORE_TRACE("Shutting down..."); }

} // namespace Monado
