#pragma once

#include "texture.h"

namespace Monado {

    class Environment : public Asset {
    public:
        Ref<TextureCube> RadianceMap;
        Ref<TextureCube> IrradianceMap;

        Environment() = default;
        Environment(const Ref<TextureCube> &radianceMap, const Ref<TextureCube> &irradianceMap)
            : RadianceMap(radianceMap), IrradianceMap(irradianceMap) {}
    };

} // namespace Monado
