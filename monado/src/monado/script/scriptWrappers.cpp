#include "monado/script/scriptWrappers.h"
#include "monado/core/math/noise.h"
#include "monado/scene/scene.h"
#include "monado/scene/entity.h"
#include "monado/scene/components.h"
#include "monado/core/input.h"

#include "entt/entt.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/object.h>
#include <mono/metadata/image.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/class.h>
#include <mono/metadata/reflection.h>

namespace Monado {
    extern std::unordered_map<uint32_t, Scene *> s_ActiveScenes;
    extern std::unordered_map<MonoType *, std::function<bool(Entity &)>> s_HasComponentFuncs;
    extern std::unordered_map<MonoType *, std::function<void(Entity &)>> s_CreateComponentFuncs;
} // namespace Monado

namespace Monado {
    namespace Script {

        enum class ComponentID { None = 0, Transform = 1, Mesh = 2, Script = 3, SpriteRenderer = 4 };

        ////////////////////////////////////////////////////////////////
        // Math ////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////

        float Monado_Noise_PerlinNoise(float x, float y) { return Noise::PerlinNoise(x, y); }

        ////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////
        // Input ///////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////

        bool Monado_Input_IsKeyPressed(KeyCode key) { return Input::IsKeyPressed(key); }

        ////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////
        // Entity //////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////

        void Monado_Entity_GetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4 *outTransform) {
            MND_CORE_ASSERT(s_ActiveScenes.find(sceneID) != s_ActiveScenes.end(), "Invalid Scene ID!");

            Scene *scene = s_ActiveScenes[sceneID];
            Entity entity((entt::entity)entityID, scene);
            auto &transformComponent = entity.GetComponent<TransformComponent>();
            memcpy(outTransform, glm::value_ptr(transformComponent.Transform), sizeof(glm::mat4));
        }

        void Monado_Entity_SetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4 *inTransform) {
            MND_CORE_ASSERT(s_ActiveScenes.find(sceneID) != s_ActiveScenes.end(), "Invalid Scene ID!");

            Scene *scene = s_ActiveScenes[sceneID];
            Entity entity((entt::entity)entityID, scene);
            auto &transformComponent = entity.GetComponent<TransformComponent>();
            memcpy(glm::value_ptr(transformComponent.Transform), inTransform, sizeof(glm::mat4));
        }

        void Monado_Entity_CreateComponent(uint32_t sceneID, uint32_t entityID, void *type) {
            MND_CORE_ASSERT(s_ActiveScenes.find(sceneID) != s_ActiveScenes.end(), "Invalid Scene ID!");
            MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);

            Scene *scene = s_ActiveScenes[sceneID];
            Entity entity((entt::entity)entityID, scene);
            s_CreateComponentFuncs[monoType](entity);
        }

        bool Monado_Entity_HasComponent(uint32_t sceneID, uint32_t entityID, void *type) {
            MND_CORE_ASSERT(s_ActiveScenes.find(sceneID) != s_ActiveScenes.end(), "Invalid Scene ID!");
            MonoType *monoType = mono_reflection_type_get_type((MonoReflectionType *)type);

            Scene *scene = s_ActiveScenes[sceneID];
            Entity entity((entt::entity)entityID, scene);
            bool result = s_HasComponentFuncs[monoType](entity);
            return result;
        }

        void *Monado_MeshComponent_GetMesh(uint32_t sceneID, uint32_t entityID) {
            MND_CORE_ASSERT(s_ActiveScenes.find(sceneID) != s_ActiveScenes.end(), "Invalid Scene ID!");

            Scene *scene = s_ActiveScenes[sceneID];
            Entity entity((entt::entity)entityID, scene);
            auto &meshComponent = entity.GetComponent<MeshComponent>();
            return new Ref<Mesh>(meshComponent.Mesh);
        }

        void Monado_MeshComponent_SetMesh(uint32_t sceneID, uint32_t entityID, Ref<Mesh> *inMesh) {
            MND_CORE_ASSERT(s_ActiveScenes.find(sceneID) != s_ActiveScenes.end(), "Invalid Scene ID!");

            Scene *scene = s_ActiveScenes[sceneID];
            Entity entity((entt::entity)entityID, scene);
            auto &meshComponent = entity.GetComponent<MeshComponent>();
            meshComponent.Mesh = inMesh ? *inMesh : nullptr;
        }

        Ref<Mesh> *Monado_Mesh_Constructor(MonoString *filepath) {
            return new Ref<Mesh>(new Mesh(mono_string_to_utf8(filepath)));
        }

        void Monado_Mesh_Destructor(Ref<Mesh> *_this) {
            Ref<Mesh> *instance = (Ref<Mesh> *)_this;
            delete _this;
        }

        Ref<Material> *Monado_Mesh_GetMaterial(Ref<Mesh> *inMesh) {
            Ref<Mesh> &mesh = *(Ref<Mesh> *)inMesh;
            return new Ref<Material>(mesh->GetMaterial());
        }

        Ref<MaterialInstance> *Monado_Mesh_GetMaterialByIndex(Ref<Mesh> *inMesh, int index) {
            Ref<Mesh> &mesh = *(Ref<Mesh> *)inMesh;
            const auto &materials = mesh->GetMaterials();

            MND_CORE_ASSERT(index < materials.size());
            return new Ref<MaterialInstance>(materials[index]);
        }

        int Monado_Mesh_GetMaterialCount(Ref<Mesh> *inMesh) {
            Ref<Mesh> &mesh = *(Ref<Mesh> *)inMesh;
            const auto &materials = mesh->GetMaterials();
            return materials.size();
        }

        void *Monado_Texture2D_Constructor(uint32_t width, uint32_t height) {
            auto result = Texture2D::Create(TextureFormat::RGBA, width, height);
            return new Ref<Texture2D>(result);
        }

        void Monado_Texture2D_Destructor(Ref<Texture2D> *_this) { delete _this; }

        void Monado_Texture2D_SetData(Ref<Texture2D> *_this, MonoArray *inData, int32_t count) {
            Ref<Texture2D> &instance = *_this;

            uint32_t dataSize = count * sizeof(glm::vec4) / 4;

            instance->Lock();
            Buffer buffer = instance->GetWriteableBuffer();
            MND_CORE_ASSERT(dataSize <= buffer.Size);
            // Convert RGBA32F color to RGBA8
            uint8_t *pixels = (uint8_t *)buffer.Data;
            uint32_t index = 0;
            for (int i = 0; i < instance->GetWidth() * instance->GetHeight(); i++) {
                glm::vec4 &value = mono_array_get(inData, glm::vec4, i);
                *pixels++ = (uint32_t)(value.x * 255.0f);
                *pixels++ = (uint32_t)(value.y * 255.0f);
                *pixels++ = (uint32_t)(value.z * 255.0f);
                *pixels++ = (uint32_t)(value.w * 255.0f);
            }

            instance->Unlock();
        }

        void Monado_Material_Destructor(Ref<Material> *_this) { delete _this; }

        void Monado_Material_SetFloat(Ref<Material> *_this, MonoString *uniform, float value) {
            Ref<Material> &instance = *(Ref<Material> *)_this;
            instance->Set(mono_string_to_utf8(uniform), value);
        }

        void Monado_Material_SetTexture(Ref<Material> *_this, MonoString *uniform, Ref<Texture2D> *texture) {
            Ref<Material> &instance = *(Ref<Material> *)_this;
            instance->Set(mono_string_to_utf8(uniform), *texture);
        }

        void Monado_MaterialInstance_Destructor(Ref<MaterialInstance> *_this) { delete _this; }

        void Monado_MaterialInstance_SetFloat(Ref<MaterialInstance> *_this, MonoString *uniform, float value) {
            Ref<MaterialInstance> &instance = *(Ref<MaterialInstance> *)_this;
            instance->Set(mono_string_to_utf8(uniform), value);
        }

        void Monado_MaterialInstance_SetVector3(Ref<MaterialInstance> *_this, MonoString *uniform, glm::vec3 *value) {
            Ref<MaterialInstance> &instance = *(Ref<MaterialInstance> *)_this;
            instance->Set(mono_string_to_utf8(uniform), *value);
        }

        void Monado_MaterialInstance_SetTexture(Ref<MaterialInstance> *_this, MonoString *uniform,
                                                Ref<Texture2D> *texture) {
            Ref<MaterialInstance> &instance = *(Ref<MaterialInstance> *)_this;
            instance->Set(mono_string_to_utf8(uniform), *texture);
        }

        void *Monado_MeshFactory_CreatePlane(float width, float height) {
            // TODO: Implement properly with MeshFactory class!
            return new Ref<Mesh>(new Mesh("alvis/assets/models/Plane1m.obj"));
        }

        ////////////////////////////////////////////////////////////////

    } // namespace Script
} // namespace Monado
