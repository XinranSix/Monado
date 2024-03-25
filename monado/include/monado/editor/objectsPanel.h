#pragma once

#include "monado/renderer/texture.h"

namespace Monado {

    class ObjectsPanel {
    public:
        ObjectsPanel();

        void OnImGuiRender();

    private:
        Ref<Texture2D> m_CubeImage;
    };

} // namespace Monado
