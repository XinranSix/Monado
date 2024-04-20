#include "monado/renderer/rendererContext.h"
#include "monado/renderer/RendererAPI.h"

#include "monado/platform/openGL/openGLContext.h"
#include "monado/platform/vulkan/vulkanContext.h"

namespace Monado {

    Ref<RendererContext> RendererContext::Create(GLFWwindow *windowHandle) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return Ref<OpenGLContext>::Create(windowHandle);
        case RendererAPIType::Vulkan: return Ref<VulkanContext>::Create(windowHandle);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

} // namespace Monado
