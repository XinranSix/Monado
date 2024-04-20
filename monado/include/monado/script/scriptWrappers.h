#pragma once

#include "monado/script/scriptEngine.h"
#include "monado/core/input.h"
#include "monado/physics/physics.h"

#include "glm/glm.hpp"

extern "C" {
typedef struct _MonoString MonoString;
typedef struct _MonoArray MonoArray;
}

namespace Monado {
    namespace Script {

        // Math
        float Monado_Noise_PerlinNoise(float x, float y);

        // Input
        bool Monado_Input_IsKeyPressed(KeyCode key);
        bool Monado_Input_IsMouseButtonPressed(MouseButton button);
        void Monado_Input_GetMousePosition(glm::vec2 *outPosition);
        void Monado_Input_SetCursorMode(CursorMode mode);
        CursorMode Monado_Input_GetCursorMode();

        // Physics
        bool Monado_Physics_Raycast(glm::vec3 *origin, glm::vec3 *direction, float maxDistance, RaycastHit *hit);
        MonoArray *Monado_Physics_OverlapBox(glm::vec3 *origin, glm::vec3 *halfSize);
        MonoArray *Monado_Physics_OverlapCapsule(glm::vec3 *origin, float radius, float halfHeight);
        MonoArray *Monado_Physics_OverlapSphere(glm::vec3 *origin, float radius);
        int32_t Monado_Physics_OverlapBoxNonAlloc(glm::vec3 *origin, glm::vec3 *halfSize, MonoArray *outColliders);
        int32_t Monado_Physics_OverlapCapsuleNonAlloc(glm::vec3 *origin, float radius, float halfHeight,
                                                      MonoArray *outColliders);
        int32_t Monado_Physics_OverlapSphereNonAlloc(glm::vec3 *origin, float radius, MonoArray *outColliders);

        // Entity
        void Monado_Entity_CreateComponent(uint64_t entityID, void *type);
        bool Monado_Entity_HasComponent(uint64_t entityID, void *type);
        uint64_t Monado_Entity_FindEntityByTag(MonoString *tag);

        void Monado_TransformComponent_GetTransform(uint64_t entityID, TransformComponent *outTransform);
        void Monado_TransformComponent_SetTransform(uint64_t entityID, TransformComponent *inTransform);
        void Monado_TransformComponent_GetTranslation(uint64_t entityID, glm::vec3 *outTranslation);
        void Monado_TransformComponent_SetTranslation(uint64_t entityID, glm::vec3 *inTranslation);
        void Monado_TransformComponent_GetRotation(uint64_t entityID, glm::vec3 *outRotation);
        void Monado_TransformComponent_SetRotation(uint64_t entityID, glm::vec3 *inRotation);
        void Monado_TransformComponent_GetScale(uint64_t entityID, glm::vec3 *outScale);
        void Monado_TransformComponent_SetScale(uint64_t entityID, glm::vec3 *inScale);

        void *Monado_MeshComponent_GetMesh(uint64_t entityID);
        void Monado_MeshComponent_SetMesh(uint64_t entityID, Ref<Mesh> *inMesh);

        void Monado_RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, glm::vec2 *impulse, glm::vec2 *offset,
                                                            bool wake);
        void Monado_RigidBody2DComponent_GetLinearVelocity(uint64_t entityID, glm::vec2 *outVelocity);
        void Monado_RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, glm::vec2 *velocity);

        RigidBodyComponent::Type Monado_RigidBodyComponent_GetBodyType(uint64_t entityID);
        void Monado_RigidBodyComponent_AddForce(uint64_t entityID, glm::vec3 *force, ForceMode foceMode);
        void Monado_RigidBodyComponent_AddTorque(uint64_t entityID, glm::vec3 *torque, ForceMode forceMode);
        void Monado_RigidBodyComponent_GetLinearVelocity(uint64_t entityID, glm::vec3 *outVelocity);
        void Monado_RigidBodyComponent_SetLinearVelocity(uint64_t entityID, glm::vec3 *velocity);
        void Monado_RigidBodyComponent_GetAngularVelocity(uint64_t entityID, glm::vec3 *outVelocity);
        void Monado_RigidBodyComponent_SetAngularVelocity(uint64_t entityID, glm::vec3 *velocity);
        void Monado_RigidBodyComponent_Rotate(uint64_t entityID, glm::vec3 *rotation);
        uint32_t Monado_RigidBodyComponent_GetLayer(uint64_t entityID);
        float Monado_RigidBodyComponent_GetMass(uint64_t entityID);
        void Monado_RigidBodyComponent_SetMass(uint64_t entityID, float mass);

        // Renderer
        // Texture2D
        void *Monado_Texture2D_Constructor(uint32_t width, uint32_t height);
        void Monado_Texture2D_Destructor(Ref<Texture2D> *_this);
        void Monado_Texture2D_SetData(Ref<Texture2D> *_this, MonoArray *inData, int32_t count);

        // Material
        void Monado_Material_Destructor(Ref<Material> *_this);
        void Monado_Material_SetFloat(Ref<Material> *_this, MonoString *uniform, float value);
        void Monado_Material_SetTexture(Ref<Material> *_this, MonoString *uniform, Ref<Texture2D> *texture);

        void Monado_MaterialInstance_Destructor(Ref<Material> *_this);
        void Monado_MaterialInstance_SetFloat(Ref<Material> *_this, MonoString *uniform, float value);
        void Monado_MaterialInstance_SetVector3(Ref<Material> *_this, MonoString *uniform, glm::vec3 *value);
        void Monado_MaterialInstance_SetVector4(Ref<Material> *_this, MonoString *uniform, glm::vec4 *value);
        void Monado_MaterialInstance_SetTexture(Ref<Material> *_this, MonoString *uniform, Ref<Texture2D> *texture);

        // Mesh
        Ref<Mesh> *Monado_Mesh_Constructor(MonoString *filepath);
        void Monado_Mesh_Destructor(Ref<Mesh> *_this);
        Ref<Material> *Monado_Mesh_GetMaterial(Ref<Mesh> *inMesh);
        Ref<Material> *Monado_Mesh_GetMaterialByIndex(Ref<Mesh> *inMesh, int index);
        int Monado_Mesh_GetMaterialCount(Ref<Mesh> *inMesh);

        void *Monado_MeshFactory_CreatePlane(float width, float height);
    } // namespace Script
} // namespace Monado
