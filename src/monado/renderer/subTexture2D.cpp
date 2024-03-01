#include "monado/renderer/subTexture2D.h"

namespace Monado {
    SubTexture2D::SubTexture2D(const std::shared_ptr<Texture2D> &textureAtlas, const glm::vec2 &minUV,
                               const glm::vec2 &maxUV)
        : m_TextureAtlas(textureAtlas) {
        // 顺序是: { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }
        m_TexCoords[0] = minUV;
        m_TexCoords[1] = { maxUV.x, minUV.y };
        m_TexCoords[2] = { minUV.x, maxUV.y };
        m_TexCoords[3] = maxUV;
    }
} // namespace Monado
