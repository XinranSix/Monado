#include "monado/renderer/buffer.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/vertexArray.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Monado {
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    Ref<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    Ref<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Monado
