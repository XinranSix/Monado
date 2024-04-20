#include "monado/renderer/indexBuffer.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/platform/opengl/openGLIndexBuffer.h"
#include "monado/platform/vulkan/vulkanIndexBuffer.h"

namespace Monado {

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t size) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::Create(size);
        case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::Create(size);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(void *data, uint32_t size) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return Ref<OpenGLIndexBuffer>::Create(data, size);
        case RendererAPIType::Vulkan: return Ref<VulkanIndexBuffer>::Create(data, size);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

} // namespace Monado
