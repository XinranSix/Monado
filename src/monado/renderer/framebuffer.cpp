#include "monado/renderer/framebuffer.h"

#include "monado/renderer/renderer.h"
#include "platform/openGL/openGLFramebuffer.h"

namespace Monado {
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
        }

        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

} // namespace Monado
