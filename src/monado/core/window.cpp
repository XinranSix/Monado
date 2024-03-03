#include "monado/core/window.h"
#include "platform/windows/windowsWindow.h"

#ifdef MND_PLATFORM_WINDOWS
    #include "platform/windows/windowsWindow.h"
#endif

namespace Monado {
    Scope<Window> Window::Create(const WindowProps &props) {
#ifdef MND_PLATFORM_WINDOWS
        return CreateScope<WindowsWindow>(props);
#else
        MND_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }

} // namespace Monado
