#pragma once

#include "assetEditorPanel.h"
#include "monado/renderer/mesh.h"

namespace Monado {

    class PhysicsMaterialEditor : public AssetEditor {
    public:
        PhysicsMaterialEditor();

        virtual void SetAsset(const Ref<Asset> &asset) override { m_Asset = (Ref<PhysicsMaterial>)asset; }

    private:
        virtual void Render() override;

    private:
        Ref<PhysicsMaterial> m_Asset;
    };

    class TextureViewer : public AssetEditor {
    public:
        TextureViewer();

        virtual void SetAsset(const Ref<Asset> &asset) override { m_Asset = (Ref<Texture>)asset; }

    private:
        virtual void Render() override;

    private:
        Ref<Texture> m_Asset;
    };

} // namespace Monado
