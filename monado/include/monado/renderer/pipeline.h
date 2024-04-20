#pragma once

#include "monado/core/ref.h"

// #include "rendererAPI.h"
#include "vertexBuffer.h"
#include "shader.h"
#include "monado/renderer/renderPass.h"

namespace Monado {

    struct PipelineSpecification {
        Ref<Shader> Shader;
        VertexBufferLayout Layout;
        Ref<RenderPass> RenderPass;

        std::string DebugName;
    };

    class Pipeline : public RefCounted {
    public:
        virtual ~Pipeline() = default;

        virtual PipelineSpecification &GetSpecification() = 0;
        virtual const PipelineSpecification &GetSpecification() const = 0;

        virtual void Invalidate() = 0;

        // TEMP: remove this when render command buffers are a thing
        virtual void Bind() = 0;

        static Ref<Pipeline> Create(const PipelineSpecification &spec);
    };

} // namespace Monado
