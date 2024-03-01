#pragma once
#include "rendererAPI.h"

#include <memory>

namespace Monado {
    class RenderCommand {
    public:
        static void Init();
        static void DrawIndexed(const std::shared_ptr<VertexArray> &, uint32_t count = 0);
        static void Clear();
        static void SetClearColor(const glm::vec4 &);

    private:
        static RendererAPI *s_RendererAPI;
    };
} // namespace Monado
