#pragma once

#include "renderer.h"

namespace Monado {

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}

        virtual void SetData(void *buffer, unsigned int size, unsigned int offset = 0) = 0;
        virtual void Bind() const = 0;

        virtual unsigned int GetSize() const = 0;
        virtual RendererID GetRendererID() const = 0;

        static VertexBuffer *Create(unsigned int size = 0);
    };

    class IndexBuffer {
    public:
        virtual ~IndexBuffer() {}

        virtual void SetData(void *buffer, unsigned int size, unsigned int offset = 0) = 0;
        virtual void Bind() const = 0;

        virtual unsigned int GetSize() const = 0;
        virtual RendererID GetRendererID() const = 0;

        static IndexBuffer *Create(unsigned int size = 0);
    };

} // namespace Monado
