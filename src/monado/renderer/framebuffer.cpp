#include "monado/renderer/framebuffer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/renderer/renderer.h"
#include "platform/openGL/openGLFramebuffer.h"

#include "monado/core/core.h"
#include "monado/core/log.h"

namespace Monado {
    std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::APIType::None: {
            MONADO_CORE_ERROR("No RendererAPI selected");
            MONADO_ASSERT(false, "Error, please choose a Renderer API");
            break;
        }
        case RendererAPI::APIType::OpenGL: {
            return std::make_shared<OpenGLFramebuffer>(spec);
            break;
        }
        default: break;
        }

        return std::shared_ptr<Framebuffer>();
    }
} // namespace Monado
