#pragma once

#include "sceneCamera.h"
#include "monado/core/uuid.h"
#include "monado/renderer/texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace Monado {
    struct IDComponent {
        UUID ID;

        IDComponent() = default;
        IDComponent(UUID id): ID(id) {}
        IDComponent(const IDComponent &) = default;
    };

    struct TagComponent {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent &) = default;
        TagComponent(const std::string &tag) : Tag(tag) {}
    };

    struct TransformComponent { // 不用继承Component
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        TransformComponent() = default;
        TransformComponent(const TransformComponent &) = default; // 复制构造函数
        TransformComponent(const glm::vec3 &translation)          // 转换构造函数
            : Translation { translation } {}
        glm::mat4 GetTransform() const {
            //  glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 }) *
            //                      glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 }) *
            //                      glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct SpriteRendererComponent {
        glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent &) = default;
        SpriteRendererComponent(const glm::vec4 &color) : Color(color) {}
    };

    // struct CircleRendererComponent {
    //     glm::vec4 Color { 1.0f, 1.0f, 1.0f, 1.0f };
    //     float Thickness = 1.0f;
    //     float Fade = 0.005f;

    //     CircleRendererComponent() = default;
    //     CircleRendererComponent(const CircleRendererComponent &) = default;
    // };

    struct CameraComponent {
        SceneCamera Camera;
        bool Primary { true };
        bool FixedAspectRatio { false };

        CameraComponent() = default;
        // CameraComponent(const glm::mat4 &projection) : camera { projection } {}
        CameraComponent(const CameraComponent &) = default;
    };

    class ScriptableEntity;
    struct NativeScriptComponent {
        ScriptableEntity *Instance {};
        // 函数指针
        // 函数指针名称：InstantiateScript、指向无参数、返回ScriptableEntity指针的函数
        ScriptableEntity *(*InstantiateScript)();
        // 函数指针名称：DestroyScript、指向NativeScriptComponent指针参数、无返回参数的函数
        void (*DestroyScript)(NativeScriptComponent *);
        template <typename T>
        void Bind() {
            // 这里绑定的函数功能是：根据T动态实例化Instanse
            InstantiateScript = []() { return static_cast<ScriptableEntity *>(new T()); }; // 引用值捕获Instance
            DestroyScript = [](NativeScriptComponent *nsc) {
                delete nsc->Instance;
                nsc->Instance = nullptr;
            };
        }
    };

    struct Rigidbody2DComponent {
        enum class BodyType { Static = 0, Dynamic, Kinematic };
        BodyType Type = BodyType::Static;
        bool FixedRotation = false;

        void *RuntimeBody = nullptr;
        Rigidbody2DComponent() = default;
        Rigidbody2DComponent(const Rigidbody2DComponent &) = default;
    };

    struct BoxCollider2DComponent {
        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 0.5f, 0.5f };

        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;

        void *RuntimeFixture = nullptr;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent &) = default;
    };

    // TODO: 待完善
    // Circle��Χ��
    /* struct CircleCollider2DComponent {
        glm::vec2 Offset = { 0.0f, 0.0f };
        float Radius = 0.5f;

        // TODO:�Ƶ���������
        float Density = 1.0f;              // �ܶ�,0�Ǿ�̬������
        float Friction = 0.5f;             // Ħ����
        float Restitution = 0.0f;          // ������0���ᵯ����1���޵���
        float RestitutionThreshold = 0.5f; // ��ԭ�ٶ���ֵ����������ٶȵ���ײ�ͻᱻ�ָ�ԭ״���ᷴ������

        // ����ʱ������������ÿһ֡�������������ܻ�䣬���Ա���Ϊ����,��δʹ��
        void *RuntimeFixture = nullptr;

        CircleCollider2DComponent() = default;
        CircleCollider2DComponent(const CircleCollider2DComponent &) = default;
    };
    // �ű���� */
    // TODO: 待完善
    /*  struct ScriptComponent {
         std::string ClassName;

         ScriptComponent() = default;
         ScriptComponent(const ScriptComponent &) = default;
     }; */
    template <typename... Component>
    struct ComponentGroup {};
    // (except IDComponent and TagComponent)
    /*     using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent, CircleRendererComponent,
                                             CameraComponent, ScriptComponent, NativeScriptComponent,
       Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent>; */
    using AllComponents = ComponentGroup<TransformComponent, SpriteRendererComponent, CameraComponent, ScriptableEntity,
                                         NativeScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent>;
} // namespace Monado
