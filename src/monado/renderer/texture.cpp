#include "monado/renderer/texture.h"

#include "monado/renderer/renderer.h"
#include "platform/openGL/openGLTexture.h"

namespace Monado {

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
        }
        MND_CORE_ASSERT(false, "UnKnown RendererAPI!");
        return nullptr;
    }
    Ref<Texture2D> Texture2D::Create(const std::string &path) {
        // ������shared_ptr,������Ҫstd::make_shared<OpenGLTexture2D>(path);��ʼ��
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
        }
        MND_CORE_ASSERT(false, "UnKnown RendererAPI!");
        return nullptr;
    }
} // namespace Monado
