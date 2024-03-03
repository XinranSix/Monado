#include "platform/opengl/openGLUniformBuffer.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {
    OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding) {
        // GPU���������������ҷ��ػ�������
        glCreateBuffers(1, &m_RendererID);
        // ����������������
        glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO:investigate usage hint
        // ����glsl�����õ�bingding 0�Ż������������Ļ�����m_RendererID��ϵ����
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
        /*	 ����ʹ��0�ŵ�uniform������
                layout(std140, binding = 0) uniform Camera
        */
    }
    OpenGLUniformBuffer::~OpenGLUniformBuffer() { glDeleteBuffers(1, &m_RendererID); }
    void OpenGLUniformBuffer::SetData(const void *data, uint32_t size, uint32_t offset) {
        // �ϴ����ݸ�m_RendererID�Ż������ɣ�ʵ���GPU��bingding�Ż�����
        glNamedBufferSubData(m_RendererID, offset, size, data);
    }
} // namespace Monado
