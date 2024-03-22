#include "monado/platform/opengl/openGLBuffer.h"
#include "monado/renderer/renderer.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    //////////////////////////////////////////////////////////////////////////////////
    // VertexBuffer
    //////////////////////////////////////////////////////////////////////////////////

    static GLenum OpenGLUsage(VertexBufferUsage usage) {
        switch (usage) {
        case VertexBufferUsage::Static: return GL_STATIC_DRAW;
        case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        }
        MND_CORE_ASSERT(false, "Unknown vertex buffer usage");
        return 0;
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(void *data, uint32_t size, VertexBufferUsage usage)
        : m_Size(size), m_Usage(usage) {
        m_LocalData = Buffer::Copy(data, size);

        Renderer::Submit([=]() {
            glCreateBuffers(1, &m_RendererID);
            glNamedBufferData(m_RendererID, m_Size, m_LocalData.Data, OpenGLUsage(m_Usage));
        });
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage) : m_Size(size), m_Usage(usage) {
        Renderer::Submit([this]() {
            glCreateBuffers(1, &m_RendererID);
            glNamedBufferData(m_RendererID, m_Size, nullptr, OpenGLUsage(m_Usage));
        });
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        Renderer::Submit([this]() { glDeleteBuffers(1, &m_RendererID); });
    }

    void OpenGLVertexBuffer::SetData(void *data, uint32_t size, uint32_t offset) {
        m_LocalData = Buffer::Copy(data, size);
        m_Size = size;
        Renderer::Submit([this, offset]() { glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data); });
    }

    void OpenGLVertexBuffer::Bind() const {
        Renderer::Submit([this]() { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); });
    }

    //////////////////////////////////////////////////////////////////////////////////
    // IndexBuffer
    //////////////////////////////////////////////////////////////////////////////////

    OpenGLIndexBuffer::OpenGLIndexBuffer(void *data, uint32_t size) : m_RendererID(0), m_Size(size) {
        m_LocalData = Buffer::Copy(data, size);

        Ref<OpenGLIndexBuffer> instance = this;
        Renderer::Submit([instance]() mutable {
            glCreateBuffers(1, &instance->m_RendererID);
            glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data, GL_STATIC_DRAW);
        });
    }

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size) : m_Size(size) {
        // m_LocalData = Buffer(size);

        Ref<OpenGLIndexBuffer> instance = this;
        Renderer::Submit([instance]() mutable {
            glCreateBuffers(1, &instance->m_RendererID);
            glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, GL_DYNAMIC_DRAW);
        });
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        Renderer::Submit([this]() { glDeleteBuffers(1, &m_RendererID); });
    }

    void OpenGLIndexBuffer::SetData(void *data, uint32_t size, uint32_t offset) {
        m_LocalData = Buffer::Copy(data, size);
        m_Size = size;
        Ref<OpenGLIndexBuffer> instance = this;
        Renderer::Submit([instance, offset]() {
            glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data);
        });
    }

    void OpenGLIndexBuffer::Bind() const {
        Renderer::Submit([this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); });
    }

} // namespace Monado
