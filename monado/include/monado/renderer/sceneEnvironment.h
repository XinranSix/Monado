#pragma once

#include "texture.h"

namespace Monado {

    struct Environment {
        std::string FilePath;
        Ref<TextureCube> RadianceMap;
        Ref<TextureCube> IrradianceMap;

        static Environment Load(const std::string &filepath);
    };

} // namespace Monado
