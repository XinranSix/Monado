#include "platform/opengl/openGLBuffer.h"
#include "monado/debug/instrumentor.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on
namespace Monado {

    /////////////////////////////////////////////////////////////////////////////
    // VertexBuffer /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
        MND_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID); // ����һ�������������ػ�������id
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); // OpenGlʹ�������������������Ϊ�����͵�
        glBufferData(GL_ARRAY_BUFFER, size, nullptr,
                     GL_DYNAMIC_DRAW); //  creates and initializes a buffer object's data store
    }
    OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size) {
        // MND_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID); // ����һ�������������ػ�������id
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); // OpenGlʹ�������������������Ϊ�����͵�
        glBufferData(GL_ARRAY_BUFFER, size, vertices,
                     GL_STATIC_DRAW); //  creates and initializes a buffer object's data store
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        MND_PROFILE_FUNCTION();

        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLVertexBuffer::Bind() const {
        MND_PROFILE_FUNCTION();

        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); // OpenGlʹ�������������������Ϊ�����͵�
    }

    void OpenGLVertexBuffer::Unbind() const {
        MND_PROFILE_FUNCTION();
        glBindBuffer(GL_ARRAY_BUFFER, 0); // OpenGlʹ�������������������Ϊ�����͵�
    }

    void OpenGLVertexBuffer::SetData(const void *data, uint32_t size) {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        // ��ȡ����CPU�Ķ��������ϴ�OpenGL
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    /////////////////////////////////////////////////////////////////////////////
    // IndexBuffer //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count) : m_Count { count } {
        MND_PROFILE_FUNCTION();

        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        MND_PROFILE_FUNCTION();

        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLIndexBuffer::Bind() const {
        MND_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void OpenGLIndexBuffer::Unbind() const {
        MND_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
} // namespace Monado
