#pragma once

#include "monado/renderer/texture.h"
#include "monado/asset/assetManager.h"

namespace Monado {

    class ObjectsPanel {
    public:
        ObjectsPanel();

        void OnImGuiRender();

    private:
        void DrawObject(const char *label, AssetHandle handle);

    private:
        Ref<Texture2D> m_CubeImage;
    };

} // namespace Monado
