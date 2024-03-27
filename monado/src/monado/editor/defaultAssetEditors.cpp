#include "monado/editor/defaultAssetEditors.h"

namespace Monado {

    PhysicsMaterialEditor::PhysicsMaterialEditor() : AssetEditor("Edit Physics Material") {}

    void PhysicsMaterialEditor::Render() {
        if (!m_Asset)
            SetOpen(false);

        UI::BeginPropertyGrid();
        UI::Property("Static Friction", m_Asset->StaticFriction);
        UI::Property("Dynamic Friction", m_Asset->DynamicFriction);
        UI::Property("Bounciness", m_Asset->Bounciness);
        UI::EndPropertyGrid();
    }

    TextureEditor::TextureEditor() : AssetEditor("Edit Texture") {
        SetMinSize(200, 600);
        SetMaxSize(500, 1000);
    }

    void TextureEditor::Render() {
        if (!m_Asset)
            SetOpen(false);

        float textureWidth = m_Asset->GetWidth();
        float textureHeight = m_Asset->GetHeight();
        float bitsPerPixel = Texture::GetBPP(m_Asset->GetFormat());
        float imageSize = ImGui::GetWindowWidth() - 40;
        imageSize = glm::min(imageSize, 500.0F);

        ImGui::SetCursorPosX(20);
        ImGui::Image((ImTextureID)m_Asset->GetRendererID(), { imageSize, imageSize });

        UI::BeginPropertyGrid();
        UI::Property("Width", textureWidth, 0.1F, 0.0F, 0.0F, true);
        UI::Property("Height", textureHeight, 0.1F, 0.0F, 0.0F, true);
        UI::Property("Bits", bitsPerPixel, 0.1F, 0.0F, 0.0F, true);
        UI::EndPropertyGrid();
    }

} // namespace Monado
