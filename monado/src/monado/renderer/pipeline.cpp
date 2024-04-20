#include "monado/renderer/pipeline.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/platform/opengl/openGLPipeline.h"
#include "monado/platform/vulkan/vulkanPipeline.h"

namespace Monado {

    Ref<Pipeline> Pipeline::Create(const PipelineSpecification &spec) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return Ref<OpenGLPipeline>::Create(spec);
        case RendererAPIType::Vulkan: return Ref<VulkanPipeline>::Create(spec);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

} // namespace Monado
