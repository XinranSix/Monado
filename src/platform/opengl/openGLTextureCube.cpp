#include "platform/opengl/openGLTextureCube.h"
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/core/core.h"
#include "monado/core/log.h"
#include "monado/renderer/textureLoader.h"

namespace Monado {
    OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string> &facesPath) {
        unsigned int textureID {};
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        int width, height, nrChannels;
        for (unsigned int i = 0; i < facesPath.size(); i++) {
            int width = 0, height = 0, nrChannels = 0;
            unsigned char *data = TextureLoader::Load(facesPath[i].c_str(), true, width, height, nrChannels);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                             data);
                TextureLoader::Free(data);
            } else {
                MONADO_INFO("Cubemap texture failed to load at path: {0}", facesPath[i]);
                return;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLTextureCube::~OpenGLTextureCube() { glDeleteTextures(1, &m_TextureID); }

    unsigned int OpenGLTextureCube::GetWidth() { return m_Width; }

    unsigned int OpenGLTextureCube::GetHeight() { return m_Height; }

    void *OpenGLTextureCube::GetTextureId() {
        return (void *)(uint64_t)m_TextureID; // 注意是m_TextureID改为(void*), 没有取其地址
    }

    void OpenGLTextureCube::Bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + (GL_TEXTURE1 - GL_TEXTURE0) * slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
    }

    void OpenGLTextureCube::SetData(void *data, uint32_t size) {
        MONADO_ASSERT(0, "Not Implemented Yet!")

        // bpp: bytes per pixel
        uint32_t bpp = 4; // 默认是用RGBA的格式
        MONADO_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
        // 可以通过glTextureSubImage2D这个API，为Texture手动提供数据，创建这个WhiteTexture
        glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
} // namespace Monado
