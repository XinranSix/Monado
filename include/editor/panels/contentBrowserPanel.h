#pragma once

#include "monado/renderer/texture.h"

#include <filesystem>

namespace Monado {

    class ContentBrowserPanel {
    public:
        ContentBrowserPanel() ;

        void OnImGuiRender();

    private:
        std::filesystem::path m_CurrentDirectory;
        /* std::filesystem::path m_BaseDirectory;
*/
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon; 
    };

} // namespace Monado
