#include "monado/renderer/texture.h"

#include "monado/renderer/renderer.h"
#include "platform/openGL/openGLTexture.h"

namespace Monado {

    Ref<Texture2D> Texture2D::Create(const TextureSpecification &specification) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(specification);
        }

        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const std::string &path) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
        }

        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

} // namespace Monado
