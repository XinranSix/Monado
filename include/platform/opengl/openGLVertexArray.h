#pragma once

#include "monado/renderer/vertexArray.h"

namespace Monado {
    class OpenGLVertexArray : public VertexArray {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer);
        virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer);

        virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffer() const { return m_VertexBuffers; };
        virtual const Ref<IndexBuffer> &GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        uint32_t m_VertexBufferIndex = 0;
        uint32_t m_RendererID;
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };

} // namespace Monado
