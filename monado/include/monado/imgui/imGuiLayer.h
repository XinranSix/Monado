#pragma once

#include "monado/core/layer.h"

namespace Monado {

    class ImGuiLayer : public Layer {
    public:
        virtual void Begin() = 0;
        virtual void End() = 0;

        void SetDarkThemeColors();

        static ImGuiLayer *Create();
    };

} // namespace Monado
