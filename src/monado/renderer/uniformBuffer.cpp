#include "monado/renderer/uniformBuffer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/renderer/renderer.h"
#include "platform/openGL/openGLUniformBuffer.h"

#include "monado/core/core.h"
#include "monado/core/log.h"

namespace Monado {
    std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::APIType::None: {
            MONADO_CORE_ERROR("No RendererAPI selected");
            MONADO_ASSERT(false, "Error, please choose a Renderer API");
            break;
        }
        case RendererAPI::APIType::OpenGL: {
            return std::make_shared<OpenGLUniformBuffer>(size, binding);
            break;
        }
        default: break;
        }

        return std::shared_ptr<UniformBuffer>();
    }
} // namespace Monado
