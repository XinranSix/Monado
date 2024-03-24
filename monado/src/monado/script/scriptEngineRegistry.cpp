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

        mono_add_internal_call("Monado.Physics::Raycast_Native", Monado::Script::Monado_Physics_Raycast);
        mono_add_internal_call("Monado.Physics::OverlapBox_Native", Monado::Script::Monado_Physics_OverlapBox);
        mono_add_internal_call("Monado.Physics::OverlapCapsule_Native", Monado::Script::Monado_Physics_OverlapCapsule);
        mono_add_internal_call("Monado.Physics::OverlapSphere_Native", Monado::Script::Monado_Physics_OverlapSphere);
        mono_add_internal_call("Monado.Physics::OverlapBoxNonAlloc_Native",
                               Monado::Script::Monado_Physics_OverlapBoxNonAlloc);
        mono_add_internal_call("Monado.Physics::OverlapCapsuleNonAlloc_Native",
                               Monado::Script::Monado_Physics_OverlapCapsuleNonAlloc);
        mono_add_internal_call("Monado.Physics::OverlapSphereNonAlloc_Native",
                               Monado::Script::Monado_Physics_OverlapSphereNonAlloc);

        mono_add_internal_call("Monado.Entity::GetTransform_Native", Monado::Script::Monado_Entity_GetTransform);
        mono_add_internal_call("Monado.Entity::SetTransform_Native", Monado::Script::Monado_Entity_SetTransform);
        mono_add_internal_call("Monado.Entity::CreateComponent_Native", Monado::Script::Monado_Entity_CreateComponent);
        mono_add_internal_call("Monado.Entity::HasComponent_Native", Monado::Script::Monado_Entity_HasComponent);
        mono_add_internal_call("Monado.Entity::FindEntityByTag_Native", Monado::Script::Monado_Entity_FindEntityByTag);

        mono_add_internal_call("Monado.TransformComponent::GetTransform_Native",
                               Monado::Script::Monado_Entity_GetTransform);
        mono_add_internal_call("Monado.TransformComponent::SetTransform_Native",
                               Monado::Script::Monado_Entity_SetTransform);
        mono_add_internal_call("Monado.TransformComponent::GetRelativeDirection_Native",
                               Monado::Script::Monado_TransformComponent_GetRelativeDirection);
        mono_add_internal_call("Monado.TransformComponent::GetRotation_Native",
                               Monado::Script::Monado_TransformComponent_GetRotation);
        mono_add_internal_call("Monado.TransformComponent::SetRotation_Native",
                               Monado::Script::Monado_TransformComponent_SetRotation);

        mono_add_internal_call("Monado.MeshComponent::GetMesh_Native", Monado::Script::Monado_MeshComponent_GetMesh);
        mono_add_internal_call("Monado.MeshComponent::SetMesh_Native", Monado::Script::Monado_MeshComponent_SetMesh);

        mono_add_internal_call("Monado.RigidBody2DComponent::ApplyLinearImpulse_Native",
                               Monado::Script::Monado_RigidBody2DComponent_ApplyLinearImpulse);
        mono_add_internal_call("Monado.RigidBody2DComponent::GetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBody2DComponent_GetLinearVelocity);
        mono_add_internal_call("Monado.RigidBody2DComponent::SetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBody2DComponent_SetLinearVelocity);

        mono_add_internal_call("Monado.RigidBodyComponent::GetBodyType_Native",
                               Monado::Script::Monado_RigidBodyComponent_GetBodyType);
        mono_add_internal_call("Monado.RigidBodyComponent::AddForce_Native",
                               Monado::Script::Monado_RigidBodyComponent_AddForce);
        mono_add_internal_call("Monado.RigidBodyComponent::AddTorque_Native",
                               Monado::Script::Monado_RigidBodyComponent_AddTorque);
        mono_add_internal_call("Monado.RigidBodyComponent::GetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBodyComponent_GetLinearVelocity);
        mono_add_internal_call("Monado.RigidBodyComponent::SetLinearVelocity_Native",
                               Monado::Script::Monado_RigidBodyComponent_SetLinearVelocity);
        mono_add_internal_call("Monado.RigidBodyComponent::GetAngularVelocity_Native",
                               Monado::Script::Monado_RigidBodyComponent_GetAngularVelocity);
        mono_add_internal_call("Monado.RigidBodyComponent::SetAngularVelocity_Native",
                               Monado::Script::Monado_RigidBodyComponent_SetAngularVelocity);
        mono_add_internal_call("Monado.RigidBodyComponent::Rotate_Native",
                               Monado::Script::Monado_RigidBodyComponent_Rotate);
        mono_add_internal_call("Monado.RigidBodyComponent::GetLayer_Native",
                               Monado::Script::Monado_RigidBodyComponent_GetLayer);
        mono_add_internal_call("Monado.RigidBodyComponent::GetMass_Native",
                               Monado::Script::Monado_RigidBodyComponent_GetMass);
        mono_add_internal_call("Monado.RigidBodyComponent::SetMass_Native",
                               Monado::Script::Monado_RigidBodyComponent_SetMass);

        mono_add_internal_call("Monado.Input::IsKeyPressed_Native", Monado::Script::Monado_Input_IsKeyPressed);
        mono_add_internal_call("Monado.Input::IsMouseButtonPressed_Native",
                               Monado::Script::Monado_Input_IsMouseButtonPressed);
        mono_add_internal_call("Monado.Input::GetMousePosition_Native", Monado::Script::Monado_Input_GetMousePosition);
        mono_add_internal_call("Monado.Input::SetCursorMode_Native", Monado::Script::Monado_Input_SetCursorMode);
        mono_add_internal_call("Monado.Input::GetCursorMode_Native", Monado::Script::Monado_Input_GetCursorMode);

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
    }

} // namespace Monado
