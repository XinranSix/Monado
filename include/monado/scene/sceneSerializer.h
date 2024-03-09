#pragma once

#include "monado/scene/scene.h"

namespace Monado {
    class SceneSerializer {
    public:
        SceneSerializer(const Ref<Scene> &scene);

        void Serialize(const std::string &filepath);
        void SerializeRuntim(const std::string &filepath);

        bool DeSerialize(const std::string &filepath);
        bool DeSerializeRuntim(const std::string &filepath);

    private:
        Ref<Scene> m_Scene;
    };
} // namespace Monado
