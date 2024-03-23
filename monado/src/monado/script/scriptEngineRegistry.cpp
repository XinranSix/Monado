#include "monado/script/scriptEngineRegistry.h"
#include "monado/script/ScriptWrappers.h"
#include "monado/scene/entity.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/object.h>
#include <mono/metadata/image.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/class.h>
#include <mono/metadata/reflection.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace Monado {

    std::unordered_map<MonoType *, std::function<bool(Entity &)>> s_HasComponentFuncs;
    std::unordered_map<MonoType *, std::function<void(Entity &)>> s_CreateComponentFuncs;

    extern MonoImage *s_CoreAssemblyImage;

#define Component_RegisterType(Type)                                                                                   \
    {                                                                                                                  \
        MonoType *type = mono_reflection_type_from_name((char *)("Monado." #Type), s_CoreAssemblyImage);               \
        if (type) {                                                                                                    \
            uint32_t id = mono_type_get_type(type);                                                                    \
            s_HasComponentFuncs[type] = [](Entity &entity) { return entity.HasComponent<Type>(); };                    \
            s_CreateComponentFuncs[type] = [](Entity &entity) { entity.AddComponent<Type>(); };                        \
        } else {                                                                                                       \
            MND_CORE_ERROR("No C# component class found for " #Type "!");                                              \
        }                                                                                                              \
    }

    static void InitComponentTypes() {
        Component_RegisterType(TagComponent);
        Component_RegisterType(TransformComponent);
        Component_RegisterType(MeshComponent);
        Component_RegisterType(ScriptComponent);
        Component_RegisterType(CameraComponent);
        Component_RegisterType(SpriteRendererComponent);
        Component_RegisterType(RigidBody2DComponent);
        Component_RegisterType(BoxCollider2DComponent);
        Component_RegisterType(RigidBodyComponent);
        Component_RegisterType(BoxColliderComponent);
        Component_RegisterType(SphereColliderComponent);
    }

    void ScriptEngineRegistry::RegisterAll() {
        InitComponentTypes();

        mono_add_internal_call("Monado.Noise::PerlinNoise_Native", Monado::Script::Monado_Noise_PerlinNoise);

        mono_add_internal_call("Monado.Entity::GetTransform_Native", Monado::Script::Monado_Entity_GetTransform);
        mono_add_internal_call("Monado.Entity::SetTransform_Native", Monado::Script::Monado_Entity_SetTransform);
        mono_add_internal_call("Monado.Entity::GetForwardDirection_Native",
                               Monado::Script::Monado_Entity_GetForwardDirection);
        mono_add_internal_call("Monado.Entity::GetRightDirection_Native",
                               Monado::Script::Monado_Entity_GetRightDirection);
        mono_add_internal_call("Monado.Entity::GetUpDirection_Native", Monado::Script::Monado_Entity_GetUpDirection);
        mono_add_internal_call("Monado.Entity::CreateComponent_Native", Monado::Script::Monado_Entity_CreateComponent);
        mono_add_internal_call("Monado.Entity::HasComponent_Native", Monado::Script::Monado_Entity_HasComponent);
        mono_add_internal_call("Monado.Entity::FindEntityByTag_Native", Monado::Script::Monado_Entity_FindEntityByTag);

        mono_add_internal_call("Monado.MeshComponent::GetMesh_Native", Monado::Script::Monado_MeshComponent_GetMesh);
        mono_add_internal_call("Monado.MeshComponent::SetMesh_Native", Monado::Script::Monado_MeshComponent_SetMesh);

        mono_add_internal_call("Monado.RigidBody2DComponent::ApplyLinearImpulse_Native",
                               Monado::Script::Monado_RigidBody2DComponent_ApplyLinearImpulse);
        mono_add_internal_call("Monado.RigidBody2DComponent::GetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBody2DComponent_GetLinearVelocity);
        mono_add_internal_call("Monado.RigidBody2DComponent::SetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBody2DComponent_SetLinearVelocity);

        mono_add_internal_call("Monado.RigidBodyComponent::AddForce_Native",
                               Monado::Script::Monado_RigidBodyComponent_AddForce);
        mono_add_internal_call("Monado.RigidBodyComponent::AddTorque_Native",
                               Monado::Script::Monado_RigidBodyComponent_AddTorque);
        mono_add_internal_call("Monado.RigidBodyComponent::GetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBodyComponent_GetLinearVelocity);
        mono_add_internal_call("Monado.RigidBodyComponent::SetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBodyComponent_SetLinearVelocity);

        mono_add_internal_call("Monado.Input::IsKeyPressed_Native", Monado::Script::Monado_Input_IsKeyPressed);

        mono_add_internal_call("Monado.Texture2D::Constructor_Native", Monado::Script::Monado_Texture2D_Constructor);
        mono_add_internal_call("Monado.Texture2D::Destructor_Native", Monado::Script::Monado_Texture2D_Destructor);
        mono_add_internal_call("Monado.Texture2D::SetData_Native", Monado::Script::Monado_Texture2D_SetData);

        mono_add_internal_call("Monado.Material::Destructor_Native", Monado::Script::Monado_Material_Destructor);
        mono_add_internal_call("Monado.Material::SetFloat_Native", Monado::Script::Monado_Material_SetFloat);
        mono_add_internal_call("Monado.Material::SetTexture_Native", Monado::Script::Monado_Material_SetTexture);

        mono_add_internal_call("Monado.MaterialInstance::Destructor_Native",
                               Monado::Script::Monado_MaterialInstance_Destructor);
        mono_add_internal_call("Monado.MaterialInstance::SetFloat_Native",
                               Monado::Script::Monado_MaterialInstance_SetFloat);
        mono_add_internal_call("Monado.MaterialInstance::SetVector3_Native",
                               Monado::Script::Monado_MaterialInstance_SetVector3);
        mono_add_internal_call("Monado.MaterialInstance::SetVector4_Native",
                               Monado::Script::Monado_MaterialInstance_SetVector4);
        mono_add_internal_call("Monado.MaterialInstance::SetTexture_Native",
                               Monado::Script::Monado_MaterialInstance_SetTexture);

        mono_add_internal_call("Monado.Mesh::Constructor_Native", Monado::Script::Monado_Mesh_Constructor);
        mono_add_internal_call("Monado.Mesh::Destructor_Native", Monado::Script::Monado_Mesh_Destructor);
        mono_add_internal_call("Monado.Mesh::GetMaterial_Native", Monado::Script::Monado_Mesh_GetMaterial);
        mono_add_internal_call("Monado.Mesh::GetMaterialByIndex_Native",
                               Monado::Script::Monado_Mesh_GetMaterialByIndex);
        mono_add_internal_call("Monado.Mesh::GetMaterialCount_Native", Monado::Script::Monado_Mesh_GetMaterialCount);

        mono_add_internal_call("Monado.MeshFactory::CreatePlane_Native",
                               Monado::Script::Monado_MeshFactory_CreatePlane);

        // static bool IsKeyPressed(KeyCode key) { return s_Instance->IsKeyPressedImpl(key); }
        //
        // static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
        // static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
        // static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
        // static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
    }

} // namespace Monado
