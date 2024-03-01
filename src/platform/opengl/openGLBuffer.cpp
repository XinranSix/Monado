#include "platform/opengl/openGLBuffer.h"
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {
    OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size) {
        glCreateBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); // 从CPU传入了GPU
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
        glCreateBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // 从CPU传入了GPU
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() { glDeleteBuffers(1, &m_VertexBuffer); }

    void OpenGLVertexBuffer::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer); }

    void OpenGLVertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

    void OpenGLVertexBuffer::SetData(uint32_t pos, void *data, uint32_t len) {
        glBufferSubData(GL_ARRAY_BUFFER, pos, len, data);
    }

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t size) {
        m_Count = size / sizeof(uint32_t);
        glCreateBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW); // 从CPU传入了GPU
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() { glDeleteBuffers(1, &m_IndexBuffer); }

    void OpenGLIndexBuffer::Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer); }

    void OpenGLIndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

    uint32_t OpenGLIndexBuffer::GetCount() const { return m_Count; }
} // namespace Monado
