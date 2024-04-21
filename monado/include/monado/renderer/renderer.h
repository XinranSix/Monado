#pragma once

#include "rendererContext.h"
#include "renderCommandQueue.h"
#include "rendererAPI.h"
#include "monado/renderer/shader.h"
#include "monado/core/base.h"
#include "renderPass.h"
#include "pipeline.h"
#include "mesh.h"
#include "monado/core/application.h"
#include "rendererCapabilities.h"
#include "texture.h"
#include "sceneEnvironment.h"
#include "monado/scene/scene.h"

namespace Monado {
    class ShaderLibrary;

    struct RendererConfig {
        // "Experimental" features
        bool ComputeEnvironmentMaps = false;
    };

    class Renderer {
    public:
        typedef void (*RenderCommandFn)(void *);

        static Ref<RendererContext> GetContext() { return Application::Get().GetWindow().GetRenderContext(); }

        static void Init();
        static void Shutdown();

        static RendererCapabilities &GetCapabilities();

        static Ref<ShaderLibrary> GetShaderLibrary();

        template <typename FuncT>
        static void Submit(FuncT &&func) {
            auto renderCmd = [](void *ptr) {
                auto pFunc = (FuncT *)ptr;
                (*pFunc)();

                // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
                // however some items like uniforms which contain std::strings still exist for now
                // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
                pFunc->~FuncT();
            };
            auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
            new (storageBuffer) FuncT(std::forward<FuncT>(func));
        }

        /*static void* Submit(RenderCommandFn fn, unsigned int size)
        {
                return s_Instance->m_CommandQueue.Allocate(fn, size);
        }*/

        static void WaitAndRender();

        // ~Actual~ Renderer here... TODO: remove confusion later
        static void BeginRenderPass(Ref<RenderPass> renderPass, bool clear = true);
        static void EndRenderPass();

        static void BeginFrame();
        static void EndFrame();

        static void SetSceneEnvironment(Ref<Environment> environment, Ref<Image2D> shadow);
        static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string &filepath);

        // class Pipeline;
        // class Mesh;
        // class Material;
        // class AABB;
        static void RenderMesh(Ref<Monado::Pipeline> pipeline, Ref<Monado::Mesh> mesh, const glm::mat4 &transform);
        static void RenderMeshWithoutMaterial(Ref<Pipeline> pipeline, Ref<Monado::Mesh> mesh, const glm::mat4 &transform);
        static void RenderQuad(Ref<Monado::Pipeline> pipeline, Ref<Monado::Material> material, const glm::mat4 &transform);
        static void SubmitFullscreenQuad(Ref<Pipeline> pipeline, Ref<Monado::Material> material);

        static void SubmitQuad(Ref<Monado::Material> material, const glm::mat4 &transform = glm::mat4(1.0f));
        static void SubmitMesh(Ref<Monado::Mesh> mesh, const glm::mat4 &transform, Ref<Monado::Material> overrideMaterial = nullptr);

        static void DrawAABB(const AABB &aabb, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(1.0f));
        static void DrawAABB(Ref<Monado::Mesh> mesh, const glm::mat4 &transform, const glm::vec4 &color = glm::vec4(1.0f));

        static Ref<Texture2D> GetWhiteTexture();
        static Ref<TextureCube> GetBlackCubeTexture();
        static Ref<Environment> GetEmptyEnvironment();

        static void RegisterShaderDependency(Ref<Monado::Shader> shader, Ref<Monado::Pipeline> pipeline);
        static void RegisterShaderDependency(Ref<Monado::Shader> shader, Ref<Monado::Material> material);
        static void OnShaderReloaded(size_t hash);

        static RendererConfig &GetConfig();

    private:
        static RenderCommandQueue &GetRenderCommandQueue();
    };

    namespace Utils {

        inline void DumpGPUInfo() {
            auto &caps = Renderer::GetCapabilities();
            MND_CORE_TRACE("GPU Info:");
            MND_CORE_TRACE("  Vendor: {0}", caps.Vendor);
            MND_CORE_TRACE("  Device: {0}", caps.Device);
            MND_CORE_TRACE("  Version: {0}", caps.Version);
        }

    } // namespace Utils

} // namespace Monado
