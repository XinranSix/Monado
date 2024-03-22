#pragma once

#include "monado/script/scriptEngine.h"
#include "monado/core/keyCodes.h"

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

        // Entity
        void Monado_Entity_GetTransform(uint64_t entityID, glm::mat4 *outTransform);
        void Monado_Entity_SetTransform(uint64_t entityID, glm::mat4 *inTransform);
        void Monado_Entity_CreateComponent(uint64_t entityID, void *type);
        bool Monado_Entity_HasComponent(uint64_t entityID, void *type);

        void *Monado_MeshComponent_GetMesh(uint64_t entityID);
        void Monado_MeshComponent_SetMesh(uint64_t entityID, Ref<Mesh> *inMesh);

        // Renderer
        // Texture2D
        void *Monado_Texture2D_Constructor(uint32_t width, uint32_t height);
        void Monado_Texture2D_Destructor(Ref<Texture2D> *_this);
        void Monado_Texture2D_SetData(Ref<Texture2D> *_this, MonoArray *inData, int32_t count);

        // Material
        void Monado_Material_Destructor(Ref<Material> *_this);
        void Monado_Material_SetFloat(Ref<Material> *_this, MonoString *uniform, float value);
        void Monado_Material_SetTexture(Ref<Material> *_this, MonoString *uniform, Ref<Texture2D> *texture);

        void Monado_MaterialInstance_Destructor(Ref<MaterialInstance> *_this);
        void Monado_MaterialInstance_SetFloat(Ref<MaterialInstance> *_this, MonoString *uniform, float value);
        void Monado_MaterialInstance_SetVector3(Ref<MaterialInstance> *_this, MonoString *uniform, glm::vec3 *value);
        void Monado_MaterialInstance_SetTexture(Ref<MaterialInstance> *_this, MonoString *uniform,
                                                Ref<Texture2D> *texture);

        // Mesh
        Ref<Mesh> *Monado_Mesh_Constructor(MonoString *filepath);
        void Monado_Mesh_Destructor(Ref<Mesh> *_this);
        Ref<Material> *Monado_Mesh_GetMaterial(Ref<Mesh> *inMesh);
        Ref<MaterialInstance> *Monado_Mesh_GetMaterialByIndex(Ref<Mesh> *inMesh, int index);
        int Monado_Mesh_GetMaterialCount(Ref<Mesh> *inMesh);

        void *Monado_MeshFactory_CreatePlane(float width, float height);
    } // namespace Script
} // namespace Monado
