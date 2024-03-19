#pragma once

#include <filesystem>

#include "monado/core/base.h"
#include "monado/renderer/texture.h"

namespace Monado {

    struct MSDFData;

    class Font {
    public:
        Font(const std::filesystem::path &font);
        ~Font();

        Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

    private:
        MSDFData *m_Data;
        Ref<Texture2D> m_AtlasTexture;
    };

} // namespace Monado
