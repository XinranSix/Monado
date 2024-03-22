#pragma once

#include <glm/glm.hpp>

#include "monado/renderer/texture.h"
#include "monado/renderer/mesh.h"
#include "sceneCamera.h"
#include "monado/core/uuid.h"

namespace Monado {

    struct IDComponent {
        UUID ID = 0;
    };

    struct TagComponent {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &other) : Tag(other.Tag) {}
        TagComponent(const std::string &tag) : Tag(tag) {}

        operator std::string &() { return Tag; }
        operator const std::string &() const { return Tag; }
    };

    struct TransformComponent {
        glm::mat4 Transform;

        TransformComponent() = default;
        TransformComponent(const TransformComponent &other) : Transform(other.Transform) {}
        TransformComponent(const glm::mat4 &transform) : Transform(transform) {}

        operator glm::mat4 &() { return Transform; }
        operator const glm::mat4 &() const { return Transform; }
    };

    struct MeshComponent {
        Ref<Monado::Mesh> Mesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent &other) : Mesh(other.Mesh) {}
        MeshComponent(const Ref<Monado::Mesh> &mesh) : Mesh(mesh) {}

        operator Ref<Monado::Mesh>() { return Mesh; }
    };

    struct ScriptComponent {
        std::string ModuleName;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent &other) : ModuleName(other.ModuleName) {}
        ScriptComponent(const std::string &moduleName) : ModuleName(moduleName) {}
    };

    struct CameraComponent {
        SceneCamera Camera;
        bool Primary = true;

        CameraComponent() = default;
        CameraComponent(const CameraComponent &other) : Camera(other.Camera), Primary(other.Primary) {}

        operator SceneCamera &() { return Camera; }
        operator const SceneCamera &() const { return Camera; }
    };

    struct SpriteRendererComponent {
        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent &other)
            : Color(other.Color), Texture(other.Texture), TilingFactor(other.TilingFactor) {}
    };

} // namespace Monado
