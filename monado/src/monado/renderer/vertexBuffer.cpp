#include "monado/renderer/vertexBuffer.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/rendererAPI.h"

#include "monado/platform/opengl/openGLVertexBuffer.h"
#include "monado/platform/vulkan/vulkanVertexBuffer.h"

namespace Monado {

    Ref<VertexBuffer> VertexBuffer::Create(void *data, uint32_t size, VertexBufferUsage usage) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return Ref<OpenGLVertexBuffer>::Create(data, size, usage);
        case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(data, size, usage);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return Ref<OpenGLVertexBuffer>::Create(size, usage);
        case RendererAPIType::Vulkan: return Ref<VulkanVertexBuffer>::Create(size, usage);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

} // namespace Monado
