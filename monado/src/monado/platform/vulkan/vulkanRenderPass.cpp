#include "monado/platform/vulkan/vulkanRenderPass.h"

namespace Monado {

    VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification &spec) : m_Specification(spec) {}

    VulkanRenderPass::~VulkanRenderPass() {}

} // namespace Monado
