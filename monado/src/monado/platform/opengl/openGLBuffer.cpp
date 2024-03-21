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

        MND_RENDER_S({
            glCreateBuffers(1, &self->m_RendererID);
            glNamedBufferData(self->m_RendererID, self->m_Size, self->m_LocalData.Data, OpenGLUsage(self->m_Usage));
        });
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage) : m_Size(size), m_Usage(usage) {
        MND_RENDER_S({
            glCreateBuffers(1, &self->m_RendererID);
            glNamedBufferData(self->m_RendererID, self->m_Size, nullptr, OpenGLUsage(self->m_Usage));
        });
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        MND_RENDER_S({ glDeleteBuffers(1, &self->m_RendererID); });
    }

    void OpenGLVertexBuffer::SetData(void *data, uint32_t size, uint32_t offset) {
        m_LocalData = Buffer::Copy(data, size);
        m_Size = size;
        MND_RENDER_S1(offset,
                      { glNamedBufferSubData(self->m_RendererID, offset, self->m_Size, self->m_LocalData.Data); });
    }

    void OpenGLVertexBuffer::Bind() const {
        MND_RENDER_S({ glBindBuffer(GL_ARRAY_BUFFER, self->m_RendererID); });
    }

    //////////////////////////////////////////////////////////////////////////////////
    // IndexBuffer
    //////////////////////////////////////////////////////////////////////////////////

    OpenGLIndexBuffer::OpenGLIndexBuffer(void *data, uint32_t size) : m_RendererID(0), m_Size(size) {
        m_LocalData = Buffer::Copy(data, size);

        MND_RENDER_S({
            glCreateBuffers(1, &self->m_RendererID);
            glNamedBufferData(self->m_RendererID, self->m_Size, self->m_LocalData.Data, GL_STATIC_DRAW);
        });
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        MND_RENDER_S({ glDeleteBuffers(1, &self->m_RendererID); });
    }

    void OpenGLIndexBuffer::SetData(void *data, uint32_t size, uint32_t offset) {
        m_LocalData = Buffer::Copy(data, size);
        m_Size = size;
        MND_RENDER_S1(offset,
                      { glNamedBufferSubData(self->m_RendererID, offset, self->m_Size, self->m_LocalData.Data); });
    }

    void OpenGLIndexBuffer::Bind() const {
        MND_RENDER_S({ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_RendererID); });
    }

} // namespace Monado
