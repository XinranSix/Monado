#pragma once

#include <glm/glm.hpp>

#include "monado/renderer/texture.h"
#include "monado/renderer/mesh.h"
#include "sceneCamera.h"
#include "monado/core/uuid.h"
#include "monado/core/math/transform.h"

namespace Monado {

    struct IDComponent {
        UUID ID = 0;
    };

    struct TagComponent {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent &other) = default;
        TagComponent(const std::string &tag) : Tag(tag) {}

        operator std::string &() { return Tag; }
        operator const std::string &() const { return Tag; }
    };

    struct TransformComponent {
        Transform Transformation;

        TransformComponent() = default;
        TransformComponent(const TransformComponent &other) = default;
        TransformComponent(const Transform &transform) : Transformation(transform) {}

        operator Transform &() { return Transformation; }
        operator const Transform &() const { return Transformation; }
    };

    struct MeshComponent {
        Ref<Monado::Mesh> Mesh;

        MeshComponent() = default;
        MeshComponent(const MeshComponent &other) = default;
        MeshComponent(const Ref<Monado::Mesh> &mesh) : Mesh(mesh) {}

        operator Ref<Monado::Mesh>() { return Mesh; }
    };

    struct ScriptComponent {
        std::string ModuleName;

        ScriptComponent() = default;
        ScriptComponent(const ScriptComponent &other) = default;
        ScriptComponent(const std::string &moduleName) : ModuleName(moduleName) {}
    };

    struct CameraComponent {
        SceneCamera Camera;
        bool Primary = true;

        CameraComponent() = default;
        CameraComponent(const CameraComponent &other) = default;

        operator SceneCamera &() { return Camera; }
        operator const SceneCamera &() const { return Camera; }
    };

    struct SpriteRendererComponent {
        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent &other) = default;
    };

    struct RigidBody2DComponent {
        enum class Type { Static, Dynamic, Kinematic };
        Type BodyType;
        bool FixedRotation = false;

        // Storage for runtime
        void *RuntimeBody = nullptr;

        RigidBody2DComponent() = default;
        RigidBody2DComponent(const RigidBody2DComponent &other) = default;
    };

    struct BoxCollider2DComponent {
        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 1.0f, 1.0f };

        float Density = 1.0f;
        float Friction = 1.0f;

        // Storage for runtime
        void *RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &other) = default;
    };

    struct CircleCollider2DComponent {
        glm::vec2 Offset = { 0.0f, 0.0f };
        float Radius = 1.0f;

        float Density = 1.0f;
        float Friction = 1.0f;

        // Storage for runtime
        void *RuntimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &other) = default;
    };

    struct RigidBodyComponent {
        enum class Type { Static, Dynamic };
        Type BodyType;
        float Mass = 1.0F;
        bool IsKinematic = false;
        uint32_t Layer = 0;

        bool LockPositionX = false;
        bool LockPositionY = false;
        bool LockPositionZ = false;
        bool LockRotationX = false;
        bool LockRotationY = false;
        bool LockRotationZ = false;

        void *RuntimeActor = nullptr;
        int32_t EntityBufferIndex = -1;

        RigidBodyComponent() = default;
        RigidBodyComponent(const RigidBodyComponent &other) = default;
    };

    // TODO: This will eventually be a resource, but that requires object referencing through the editor
    struct PhysicsMaterialComponent {
        float StaticFriction = 1.0F;
        float DynamicFriction = 1.0F;
        float Bounciness = 1.0F;

        PhysicsMaterialComponent() = default;
        PhysicsMaterialComponent(const PhysicsMaterialComponent &other) = default;
    };

    struct BoxColliderComponent {
        glm::vec3 Size = { 1.0F, 1.0F, 1.0F };
        glm::vec3 Offset = { 0.0F, 0.0F, 0.0F };

        bool IsTrigger = false;

        // The mesh that will be drawn in the editor to show the collision bounds
        Ref<Mesh> DebugMesh;

        BoxColliderComponent() = default;
        BoxColliderComponent(const BoxColliderComponent &other) = default;
    };

    struct SphereColliderComponent {
        float Radius = 0.5F;
        bool IsTrigger = false;

        // The mesh that will be drawn in the editor to show the collision bounds
        Ref<Mesh> DebugMesh;

        SphereColliderComponent() = default;
        SphereColliderComponent(const SphereColliderComponent &other) = default;
    };

    struct CapsuleColliderComponent {
        float Radius = 0.5F;
        float Height = 1.0F;
        bool IsTrigger = false;

        Ref<Mesh> DebugMesh;

        CapsuleColliderComponent() = default;
        CapsuleColliderComponent(const CapsuleColliderComponent &other) = default;
    };

    struct MeshColliderComponent {
        Ref<Monado::Mesh> CollisionMesh;
        Ref<Monado::Mesh> ProcessedMesh;
        bool IsTrigger = false;

        MeshColliderComponent() = default;
        MeshColliderComponent(const MeshColliderComponent &other) = default;
        MeshColliderComponent(const Ref<Monado::Mesh> &mesh) : CollisionMesh(mesh) {}

        operator Ref<Monado::Mesh>() { return CollisionMesh; }
    };

} // namespace Monado
