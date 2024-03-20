#include "monado/renderer/texture.h"

#include "monado/renderer/rendererAPI.h"
#include "monado/platform/opengl/openGLTexture.h"

namespace Monado {

    Texture2D *Texture2D::Create(TextureFormat format, unsigned int width, unsigned int height) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return new OpenGLTexture2D(format, width, height);
        }
        return nullptr;
    }

    Texture2D *Texture2D::Create(const std::string &path, bool srgb) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return new OpenGLTexture2D(path, srgb);
        }
        return nullptr;
    }

    TextureCube *TextureCube::Create(const std::string &path) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return new OpenGLTextureCube(path);
        }
        return nullptr;
    }

} // namespace Monado
