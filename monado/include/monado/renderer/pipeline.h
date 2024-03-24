#pragma once

#include "monado/core/ref.h"

#include "rendererAPI.h"
#include "vertexBuffer.h"
#include "shader.h"

namespace Monado {

    struct PipelineSpecification {
        Ref<Monado::Shader> Shader;
        VertexBufferLayout Layout;
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
