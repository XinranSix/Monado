#pragma once

namespace Monado {

    class PhysicsSettingsWindow {
    public:
        static void OnImGuiRender(bool *show);
        static void RenderLayerList();
        static void RenderSelectedLayer();
    };

} // namespace Monado
