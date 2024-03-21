#include "monado/renderer/renderer.h"
#include "monado/renderer/shader.h"

namespace Monado {

    Renderer *Renderer::s_Instance = new Renderer();
    RendererAPIType RendererAPI::s_CurrentRendererAPI = RendererAPIType::OpenGL;

    void Renderer::Init() {
        s_Instance->m_ShaderLibrary = std::make_unique<ShaderLibrary>();
        MND_RENDER({ RendererAPI::Init(); });

        Renderer::GetShaderLibrary()->Load("sandbox/assets/shaders/MonadoPBR_Static.glsl");
        Renderer::GetShaderLibrary()->Load("sandbox/assets/shaders/MonadoPBR_Anim.glsl");
    }

    void Renderer::Clear() {
        MND_RENDER({ RendererAPI::Clear(0.0f, 0.0f, 0.0f, 1.0f); });
    }

    void Renderer::Clear(float r, float g, float b, float a) {
        MND_RENDER_4(r, g, b, a, { RendererAPI::Clear(r, g, b, a); });
    }

    void Renderer::ClearMagenta() { Clear(1, 0, 1); }

    void Renderer::SetClearColor(float r, float g, float b, float a) {}

    void Renderer::DrawIndexed(uint32_t count, bool depthTest) {
        MND_RENDER_2(count, depthTest, { RendererAPI::DrawIndexed(count, depthTest); });
    }

    void Renderer::WaitAndRender() { s_Instance->m_CommandQueue.Execute(); }

} // namespace Monado
