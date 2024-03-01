#include "platform/opengl/openGLVertexArray.h"
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {
    static GLenum GetShaderDataTypeToOpenGL(const ShaderDataType &type) {
        switch (type) {
        case Monado::ShaderDataType::FLOAT:
        case Monado::ShaderDataType::FLOAT2:
        case Monado::ShaderDataType::FLOAT3:
        case Monado::ShaderDataType::FLOAT4:
        case Monado::ShaderDataType::MAT3:
        case Monado::ShaderDataType::MAT4: return GL_FLOAT;
        case Monado::ShaderDataType::INT:
        case Monado::ShaderDataType::INT2:
        case Monado::ShaderDataType::INT3:
        case Monado::ShaderDataType::INT4: return GL_INT;
        case Monado::ShaderDataType::BOOL:
            return GL_BOOL;

            MONADO_ASSERT(false, "Unknown Shader Data Type");
            return GL_FALSE;
        }
        return GL_FALSE;
    }

    OpenGLVertexArray::OpenGLVertexArray() { glCreateVertexArrays(1, &m_Index); }

    OpenGLVertexArray::~OpenGLVertexArray() { glDeleteVertexArrays(1, &m_Index); }

    void OpenGLVertexArray::Bind() const { glBindVertexArray(m_Index); }

    void OpenGLVertexArray::Unbind() const { glBindVertexArray(0); }

    void OpenGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertexBuffer) {
        MONADO_CORE_ASSERT(vertexBuffer->GetBufferLayout().GetCount(), "Empty Layout in VertexBuffer!");
        // 挖VBO的数据到VAO时，要记得先Bind Vertex Array
        glBindVertexArray(m_Index);
        vertexBuffer->Bind();

        BufferLayout layout = vertexBuffer->GetBufferLayout();
        int index = 0;
        for (const BufferElement &element : layout) {
            glEnableVertexAttribArray(index);
            if (element.IsIntergerType()) {
                glVertexAttribIPointer(index, GetShaderTypeDataCount(element.GetType()),
                                       element.IsNormalized() ? GL_TRUE : GL_FALSE, layout.GetStride(),
                                       (const void *)(uint64_t)(element.GetOffset()));

                // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            } else {
                glVertexAttribPointer(index, GetShaderTypeDataCount(element.GetType()),
                                      GetShaderDataTypeToOpenGL(element.GetType()),
                                      element.IsNormalized() ? GL_TRUE : GL_FALSE, layout.GetStride(),
                                      (const void *)(uint64_t)(element.GetOffset()));
/*                 glVertexAttribPointer(index, GetShaderTypeDataCount(element.GetType()), GL_FLOAT, GL_FALSE,
                                      layout.GetStride(), (const void *)(uint64_t)(element.GetOffset())); */
            }

            index++;
        }
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> &indexBuffer) {
        // 先确保Bind
        glBindVertexArray(m_Index);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

    const std::vector<std::shared_ptr<VertexBuffer>> &OpenGLVertexArray::GetVertexBuffers() const {
        return m_VertexBuffers;
    }

    const std::shared_ptr<IndexBuffer> &OpenGLVertexArray::GetIndexBuffer() const { return m_IndexBuffer; }
} // namespace Monado
