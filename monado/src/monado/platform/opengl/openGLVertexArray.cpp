#include "monado/platform/opengl/openGLVertexArray.h"
#include "monado/core/log.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
        switch (type) {
        case Monado::ShaderDataType::Float: return GL_FLOAT;
        case Monado::ShaderDataType::Float2: return GL_FLOAT;
        case Monado::ShaderDataType::Float3: return GL_FLOAT;
        case Monado::ShaderDataType::Float4: return GL_FLOAT;
        case Monado::ShaderDataType::Mat3: return GL_FLOAT;
        case Monado::ShaderDataType::Mat4: return GL_FLOAT;
        case Monado::ShaderDataType::Int: return GL_INT;
        case Monado::ShaderDataType::Int2: return GL_INT;
        case Monado::ShaderDataType::Int3: return GL_INT;
        case Monado::ShaderDataType::Int4: return GL_INT;
        case Monado::ShaderDataType::Bool: return GL_BOOL;
        }

        MND_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray() {
        MND_RENDER_S({ glCreateVertexArrays(1, &self->m_RendererID); });
    }

    OpenGLVertexArray::~OpenGLVertexArray() {
        MND_RENDER_S({ glDeleteVertexArrays(1, &self->m_RendererID); });
    }

    void OpenGLVertexArray::Bind() const {
        MND_RENDER_S({ glBindVertexArray(self->m_RendererID); });
    }

    void OpenGLVertexArray::Unbind() const {
        MND_RENDER_S({ glBindVertexArray(0); });
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) {
        MND_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

        Bind();
        vertexBuffer->Bind();

        MND_RENDER_S1(vertexBuffer, {
            const auto &layout = vertexBuffer->GetLayout();
            for (const auto &element : layout) {
                auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
                glEnableVertexAttribArray(self->m_VertexBufferIndex);
                if (glBaseType == GL_INT) {
                    glVertexAttribIPointer(self->m_VertexBufferIndex, element.GetComponentCount(), glBaseType,
                                           layout.GetStride(), (const void *)(intptr_t)element.Offset);
                } else {
                    glVertexAttribPointer(self->m_VertexBufferIndex, element.GetComponentCount(), glBaseType,
                                          element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(),
                                          (const void *)(intptr_t)element.Offset);
                }
                self->m_VertexBufferIndex++;
            }
        });
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) {
        Bind();
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }

} // namespace Monado
