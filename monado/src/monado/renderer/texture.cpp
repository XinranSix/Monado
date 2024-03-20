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

} // namespace Monado
