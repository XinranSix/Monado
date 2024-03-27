#pragma once

#include <string>

#include "monado/core/uuid.h"

namespace Monado {

    enum class AssetType { Scene, Mesh, Texture, EnvMap, Audio, Script, PhysicsMat, Other };

    using AssetHandle = UUID;

    class Asset : public RefCounted {
    public:
        AssetHandle Handle;
        AssetType Type;

        std::string FilePath;
        std::string FileName;
        std::string Extension;
        int ParentDirectory;
        bool IsDataLoaded = false;

        virtual ~Asset() {}
    };

    class PhysicsMaterial : public Asset {
    public:
        float StaticFriction;
        float DynamicFriction;
        float Bounciness;

        PhysicsMaterial() = default;
        PhysicsMaterial(float staticFriction, float dynamicFriction, float bounciness)
            : StaticFriction(staticFriction), DynamicFriction(dynamicFriction), Bounciness(bounciness) {}
    };

} // namespace Monado
