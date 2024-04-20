#include "monado/renderer/framebuffer.h"

#include "monado/platform/opengl/openGLFramebuffer.h"
#include "monado/platform/vulkan/vulkanFramebuffer.h"

#include "monado/renderer/rendererAPI.h"

namespace Monado {

    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec) {
        Ref<Framebuffer> result = nullptr;

        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: result = Ref<OpenGLFramebuffer>::Create(spec); break;
        case RendererAPIType::Vulkan: result = Ref<VulkanFramebuffer>::Create(spec); break;
        }
        FramebufferPool::GetGlobal()->Add(result);
        return result;
    }

    FramebufferPool *FramebufferPool::s_Instance = new FramebufferPool;

    FramebufferPool::FramebufferPool(uint32_t maxFBs /* = 32 */) {}

    FramebufferPool::~FramebufferPool() {}

    std::weak_ptr<Framebuffer> FramebufferPool::AllocateBuffer() {
        // m_Pool.push_back();
        return std::weak_ptr<Framebuffer>();
    }

    void FramebufferPool::Add(const Ref<Framebuffer> &framebuffer) { m_Pool.push_back(framebuffer); }

} // namespace Monado
