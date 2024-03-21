#include "monado/renderer/renderPass.h"
#include "monado/renderer/renderer.h"
#include "monado/core/log.h"
#include "monado/platform/opengl/openGLRenderPass.h"

namespace Monado {

    Ref<RenderPass> RenderPass::Create(const RenderPassSpecification &spec) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None:
            MND_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPIType::OpenGL: return std::make_shared<OpenGLRenderPass>(spec);
        }

        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Monado
