#include "monado/renderer/material.h"

#include "monado/platform/vulkan/vulkanMaterial.h"
#include "monado/platform/opengl/openGLMaterial.h"

#include "monado/renderer/rendererAPI.h"

namespace Monado {

    Ref<Material> Material::Create(const Ref<Shader> &shader, const std::string &name) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::Vulkan: return Ref<VulkanMaterial>::Create(shader, name);
        case RendererAPIType::OpenGL: return Ref<OpenGLMaterial>::Create(shader, name);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

} // namespace Monado
