#pragma once

#include "monado/scene/scene.h"
#include "monado/renderer/mesh.h"
#include "monado/scene/components.h"
#include "renderPass.h"

namespace Monado {

    struct SceneRendererOptions {
        bool ShowGrid = true;
        bool ShowBoundingBoxes = false;
    };

    struct SceneRendererCamera {
        Monado::Camera Camera;
        glm::mat4 ViewMatrix;
        float Near, Far;
        float FOV;
    };

    class SceneRenderer {
    public:
        static void Init();

        static void SetViewportSize(uint32_t width, uint32_t height);

        static void BeginScene(const Scene *scene, const SceneRendererCamera &camera);
        static void EndScene();

        static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4 &transform = glm::mat4(1.0f),
                               Ref<MaterialInstance> overrideMaterial = nullptr);
        static void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4 &transform = glm::mat4(1.0f));
        static void SubmitColliderMesh(const BoxColliderComponent &component,
                                       const glm::mat4 &parentTransform = glm::mat4(1.0F));
        static void SubmitColliderMesh(const SphereColliderComponent &component,
                                       const glm::mat4 &parentTransform = glm::mat4(1.0F));
        static void SubmitColliderMesh(const CapsuleColliderComponent &component,
                                       const glm::mat4 &parentTransform = glm::mat4(1.0F));
        static void SubmitColliderMesh(const MeshColliderComponent &component,
                                       const glm::mat4 &parentTransform = glm::mat4(1.0F));

        static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string &filepath);

        static Ref<RenderPass> GetFinalRenderPass();
        static Ref<Texture2D> GetFinalColorBuffer();

        // TODO: Temp
        static uint32_t GetFinalColorBufferRendererID();
        static void SetFocusPoint(const glm::vec2 &point);

        static SceneRendererOptions &GetOptions();

        static void OnImGuiRender();

    private:
        static void FlushDrawList();
        static void GeometryPass();
        static void CompositePass();
        static void BloomBlurPass();

        static void ShadowMapPass();
    };

} // namespace Monado
