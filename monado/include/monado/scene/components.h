#pragma once

#include <glm/glm.hpp>

#include "monado/renderer/texture.h"
#include "monado/renderer/mesh.h"
#include "monado/renderer/camera.h"

namespace Monado {

    struct TagComponent {
        std::string Tag;

        operator std::string &() { return Tag; }
        operator const std::string &() const { return Tag; }
    };

    struct TransformComponent {
        glm::mat4 Transform;

        operator glm::mat4 &() { return Transform; }
        operator const glm::mat4 &() const { return Transform; }
    };

    struct MeshComponent {
        Ref<Monado::Mesh> Mesh;

        operator Ref<Monado::Mesh>() { return Mesh; }
    };

    struct ScriptComponent {
        // TODO: C# script
        std::string ModuleName;
    };

    struct CameraComponent {
        // OrthographicCamera Camera;
        Monado::Camera Camera;
        bool Primary = true;

        operator Monado::Camera &() { return Camera; }
        operator const Monado::Camera &() const { return Camera; }
    };

    struct SpriteRendererComponent {
        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;
    };

} // namespace Monado
