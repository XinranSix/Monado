#include "monado/renderer/Renderer.h"
#include "monado/renderer/sceneRenderer.h"
#include "monado/renderer/renderer2D.h"
#include "monado/renderer/meshFactory.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on
#include "glm/gtc/matrix_transform.hpp"

namespace Monado {

    struct SceneRendererData {
        const Scene *ActiveScene = nullptr;
        struct SceneInfo {
            SceneRendererCamera SceneCamera;

            // Resources
            Ref<MaterialInstance> SkyboxMaterial;
            Environment SceneEnvironment;
            Light ActiveLight;
        } SceneData;

        Ref<Texture2D> BRDFLUT;
        Ref<Shader> CompositeShader;

        Ref<RenderPass> GeoPass;
        Ref<RenderPass> CompositePass;

        struct DrawCommand {
            Ref<Mesh> Mesh;
            Ref<MaterialInstance> Material;
            glm::mat4 Transform;
        };
        std::vector<DrawCommand> DrawList;
        std::vector<DrawCommand> SelectedMeshDrawList;
        std::vector<DrawCommand> ColliderDrawList;

        // Grid
        Ref<MaterialInstance> GridMaterial;
        Ref<MaterialInstance> OutlineMaterial;

        Ref<MaterialInstance> ColliderMaterial;

        SceneRendererOptions Options;
    };

    static SceneRendererData s_Data;

    void SceneRenderer::Init() {
        FramebufferSpecification geoFramebufferSpec;
        geoFramebufferSpec.Width = 1280;
        geoFramebufferSpec.Height = 720;
        geoFramebufferSpec.Format = FramebufferFormat::RGBA16F;
        geoFramebufferSpec.Samples = 8;
        geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

        RenderPassSpecification geoRenderPassSpec;
        geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
        s_Data.GeoPass = RenderPass::Create(geoRenderPassSpec);

        FramebufferSpecification compFramebufferSpec;
        compFramebufferSpec.Width = 1280;
        compFramebufferSpec.Height = 720;
        compFramebufferSpec.Format = FramebufferFormat::RGBA8;
        compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

        RenderPassSpecification compRenderPassSpec;
        compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
        s_Data.CompositePass = RenderPass::Create(compRenderPassSpec);

        s_Data.CompositeShader = Shader::Create("alvis/assets/shaders/SceneComposite.glsl");
        s_Data.BRDFLUT = Texture2D::Create("alvis/assets/textures/BRDF_LUT.tga");

        // Grid
        auto gridShader = Shader::Create("alvis/assets/shaders/Grid.glsl");
        s_Data.GridMaterial = MaterialInstance::Create(Material::Create(gridShader));
        float gridScale = 16.025f, gridSize = 0.025f;
        s_Data.GridMaterial->Set("u_Scale", gridScale);
        s_Data.GridMaterial->Set("u_Res", gridSize);

        // Outline
        auto outlineShader = Shader::Create("alvis/assets/shaders/Outline.glsl");
        s_Data.OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
        s_Data.OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);

        auto colliderShader = Shader::Create("alvis/assets/shaders/Collider.glsl");
        s_Data.ColliderMaterial = MaterialInstance::Create(Material::Create(colliderShader));
        s_Data.ColliderMaterial->SetFlag(MaterialFlag::DepthTest, false);
    }

    void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height) {
        s_Data.GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
        s_Data.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
    }

    void SceneRenderer::BeginScene(const Scene *scene, const SceneRendererCamera &camera) {
        MND_CORE_ASSERT(!s_Data.ActiveScene, "");

        s_Data.ActiveScene = scene;

        s_Data.SceneData.SceneCamera = camera;
        s_Data.SceneData.SkyboxMaterial = scene->m_SkyboxMaterial;
        s_Data.SceneData.SceneEnvironment = scene->m_Environment;
        s_Data.SceneData.ActiveLight = scene->m_Light;
    }

    void SceneRenderer::EndScene() {
        MND_CORE_ASSERT(s_Data.ActiveScene, "");

        s_Data.ActiveScene = nullptr;

        FlushDrawList();
    }

    void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4 &transform, Ref<MaterialInstance> overrideMaterial) {
        // TODO: Culling, sorting, etc.
        s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
    }

    void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4 &transform) {
        s_Data.SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
    }

    void SceneRenderer::SubmitColliderMesh(const BoxColliderComponent &component, const glm::mat4 &parentTransform) {
        s_Data.ColliderDrawList.push_back(
            { component.DebugMesh, nullptr, glm::translate(parentTransform, component.Offset) });
    }

    void SceneRenderer::SubmitColliderMesh(const SphereColliderComponent &component, const glm::mat4 &parentTransform) {
        s_Data.ColliderDrawList.push_back({ component.DebugMesh, nullptr, parentTransform });
    }

    void SceneRenderer::SubmitColliderMesh(const CapsuleColliderComponent &component,
                                           const glm::mat4 &parentTransform) {
        s_Data.ColliderDrawList.push_back({ component.DebugMesh, nullptr, parentTransform });
    }

    void SceneRenderer::SubmitColliderMesh(const MeshColliderComponent &component, const glm::mat4 &parentTransform) {
        s_Data.ColliderDrawList.push_back({ component.ProcessedMesh, nullptr, parentTransform });
    }

    static Ref<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;
    std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string &filepath) {
        const uint32_t cubemapSize = 2048;
        const uint32_t irradianceMapSize = 32;

        Ref<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
        if (!equirectangularConversionShader)
            equirectangularConversionShader = Shader::Create("alvis/assets/shaders/EquirectangularToCubeMap.glsl");
        Ref<Texture2D> envEquirect = Texture2D::Create(filepath);
        MND_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

        equirectangularConversionShader->Bind();
        envEquirect->Bind();
        Renderer::Submit([envUnfiltered, cubemapSize, envEquirect]() {
            glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
            glGenerateTextureMipmap(envUnfiltered->GetRendererID());
        });

        if (!envFilteringShader)
            envFilteringShader = Shader::Create("alvis/assets/shaders/EnvironmentMipFilter.glsl");

        Ref<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);

        Renderer::Submit([envUnfiltered, envFiltered]() {
            glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
                               envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, envFiltered->GetWidth(),
                               envFiltered->GetHeight(), 6);
        });

        envFilteringShader->Bind();
        envUnfiltered->Bind();

        Renderer::Submit([envUnfiltered, envFiltered, cubemapSize]() {
            const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
            for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount();
                 level++, size /= 2) // <= ?
            {
                const GLuint numGroups = glm::max(1, size / 32);
                glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
                glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
                glDispatchCompute(numGroups, numGroups, 6);
            }
        });

        if (!envIrradianceShader)
            envIrradianceShader = Shader::Create("alvis/assets/shaders/EnvironmentIrradiance.glsl");

        Ref<TextureCube> irradianceMap =
            TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize);
        envIrradianceShader->Bind();
        envFiltered->Bind();
        Renderer::Submit([irradianceMap]() {
            glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
            glGenerateTextureMipmap(irradianceMap->GetRendererID());
        });

        return { envFiltered, irradianceMap };
    }

    void SceneRenderer::GeometryPass() {
        bool outline = s_Data.SelectedMeshDrawList.size() > 0;
        bool collider = s_Data.ColliderDrawList.size() > 0;

        if (outline || collider) {
            Renderer::Submit([]() { glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); });
        }

        Renderer::BeginRenderPass(s_Data.GeoPass);

        if (outline || collider) {
            Renderer::Submit([]() { glStencilMask(0); });
        }

        auto viewProjection =
            s_Data.SceneData.SceneCamera.Camera.GetProjectionMatrix() * s_Data.SceneData.SceneCamera.ViewMatrix;
        glm::vec3 cameraPosition = glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3];

        // Skybox
        auto skyboxShader = s_Data.SceneData.SkyboxMaterial->GetShader();
        s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
        Renderer::SubmitFullscreenQuad(s_Data.SceneData.SkyboxMaterial);

        // Render entities
        for (auto &dc : s_Data.DrawList) {
            auto baseMaterial = dc.Mesh->GetMaterial();
            baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
            baseMaterial->Set("u_CameraPosition", cameraPosition);

            // Environment (TODO: don't do this per mesh)
            baseMaterial->Set("u_EnvRadianceTex", s_Data.SceneData.SceneEnvironment.RadianceMap);
            baseMaterial->Set("u_EnvIrradianceTex", s_Data.SceneData.SceneEnvironment.IrradianceMap);
            baseMaterial->Set("u_BRDFLUTTexture", s_Data.BRDFLUT);

            // Set lights (TODO: move to light environment and don't do per mesh)
            baseMaterial->Set("lights", s_Data.SceneData.ActiveLight);

            auto overrideMaterial = nullptr; // dc.Material;
            Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
        }

        if (outline) {
            Renderer::Submit([]() {
                glStencilFunc(GL_NOTEQUAL, 1, 0xff);
                glStencilMask(0);

                glLineWidth(10);
                glEnable(GL_LINE_SMOOTH);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_DEPTH_TEST);
            });

            // Draw outline here
            s_Data.OutlineMaterial->Set("u_ViewProjection", viewProjection);
            for (auto &dc : s_Data.SelectedMeshDrawList) {
                Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
            }

            Renderer::Submit([]() {
                glPointSize(10);
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            });

            for (auto &dc : s_Data.SelectedMeshDrawList) {
                Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.OutlineMaterial);
            }

            Renderer::Submit([]() {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glStencilMask(0xff);
                glStencilFunc(GL_ALWAYS, 1, 0xff);
                glEnable(GL_DEPTH_TEST);
            });
        }

        if (collider) {
            Renderer::Submit([]() {
                glStencilFunc(GL_NOTEQUAL, 1, 0xff);
                glStencilMask(0);

                glLineWidth(1);
                glEnable(GL_LINE_SMOOTH);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_DEPTH_TEST);
            });

            s_Data.ColliderMaterial->Set("u_ViewProjection", viewProjection);
            for (auto &dc : s_Data.ColliderDrawList) {
                if (dc.Mesh)
                    Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.ColliderMaterial);
            }

            Renderer::Submit([]() {
                glPointSize(1);
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            });

            for (auto &dc : s_Data.ColliderDrawList) {
                if (dc.Mesh)
                    Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_Data.ColliderMaterial);
            }

            Renderer::Submit([]() {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glStencilMask(0xff);
                glStencilFunc(GL_ALWAYS, 1, 0xff);
                glEnable(GL_DEPTH_TEST);
            });
        }

        // Grid
        if (GetOptions().ShowGrid) {
            s_Data.GridMaterial->Set("u_ViewProjection", viewProjection);
            Renderer::SubmitQuad(s_Data.GridMaterial,
                                 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                                     glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
        }

        if (GetOptions().ShowBoundingBoxes) {
            Renderer2D::BeginScene(viewProjection);
            for (auto &dc : s_Data.DrawList)
                Renderer::DrawAABB(dc.Mesh, dc.Transform);
            Renderer2D::EndScene();
        }

        Renderer::EndRenderPass();
    }

    void SceneRenderer::CompositePass() {
        Renderer::BeginRenderPass(s_Data.CompositePass);
        s_Data.CompositeShader->Bind();
        s_Data.CompositeShader->SetFloat("u_Exposure", s_Data.SceneData.SceneCamera.Camera.GetExposure());
        s_Data.CompositeShader->SetInt(
            "u_TextureSamples", s_Data.GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
        s_Data.GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
        Renderer::SubmitFullscreenQuad(nullptr);
        Renderer::EndRenderPass();
    }

    void SceneRenderer::FlushDrawList() {
        MND_CORE_ASSERT(!s_Data.ActiveScene, "");

        GeometryPass();
        CompositePass();

        s_Data.DrawList.clear();
        s_Data.SelectedMeshDrawList.clear();
        s_Data.ColliderDrawList.clear();
        s_Data.SceneData = {};
    }

    Ref<Texture2D> SceneRenderer::GetFinalColorBuffer() {
        // return s_Data.CompositePass->GetSpecification().TargetFramebuffer;
        MND_CORE_ASSERT(false, "Not implemented");
        return nullptr;
    }

    Ref<RenderPass> SceneRenderer::GetFinalRenderPass() { return s_Data.CompositePass; }

    uint32_t SceneRenderer::GetFinalColorBufferRendererID() {
        return s_Data.CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachmentRendererID();
    }

    SceneRendererOptions &SceneRenderer::GetOptions() { return s_Data.Options; }

} // namespace Monado
