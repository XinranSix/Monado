#include "monado/platform/opengl/openGLImage.h"
#include "monado/renderer/renderer.h"

namespace Monado {

    OpenGLImage2D::OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, const void *data)
        : m_Width(width), m_Height(height), m_Format(format) {
        // TODO: Local storage should be optional
        if (data)
            m_ImageData = Buffer::Copy(data, Utils::GetImageMemorySize(format, width, height));
    }

    OpenGLImage2D::OpenGLImage2D(ImageFormat format, uint32_t width, uint32_t height, Buffer buffer)
        : m_Width(width), m_Height(height), m_Format(format), m_ImageData(buffer) {}

    OpenGLImage2D::~OpenGLImage2D() {
        // Should this be submitted?
        m_ImageData.Release();
        if (m_RendererID) {
            RendererID rendererID = m_RendererID;
            Renderer::Submit([rendererID]() { glDeleteTextures(1, &rendererID); });
        }
    }

    void OpenGLImage2D::Invalidate() {
        if (m_RendererID)
            Release();

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

        GLenum internalFormat = Utils::OpenGLImageInternalFormat(m_Format);
        uint32_t mipCount = Utils::CalculateMipCount(m_Width, m_Height);
        glTextureStorage2D(m_RendererID, mipCount, internalFormat, m_Width, m_Height);
        if (m_ImageData) {
            GLenum format = Utils::OpenGLImageFormat(m_Format);
            GLenum dataType = Utils::OpenGLFormatDataType(m_Format);
            glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, dataType, m_ImageData.Data);
            glGenerateTextureMipmap(m_RendererID); // TODO: optional
        }

        // Sampler
        // TODO: should be separate from Image2D
        glCreateSamplers(1, &m_SamplerRendererID);
        glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_MIN_FILTER,
                            m_ImageData ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(m_SamplerRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void OpenGLImage2D::Release() {
        if (m_RendererID) {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
        m_ImageData.Release();
    }

} // namespace Monado
