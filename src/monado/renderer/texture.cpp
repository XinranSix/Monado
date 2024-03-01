#include "monado/renderer/rendererAPI.h"
#include "monado/renderer/texture.h"
#include "platform/openGL/openGLTexture2D.h"
#include "platform/openGL/openGLTextureCube.h"

#include "monado/core/core.h"
#include "monado/core/log.h"

namespace Monado {

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string &path) {
        switch (RendererAPI::GetAPIType()) {
        case RendererAPI::APIType::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
        case RendererAPI::APIType::None: {
            CORE_LOG_ERROR("No RendererAPI selected");
            MONADO_ASSERT(false, "Error, please choose a Renderer API");
            return nullptr;
        }
        default: break;
        }

        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height) {
        switch (RendererAPI::GetAPIType()) {
        case RendererAPI::APIType::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
        case RendererAPI::APIType::None: {
            CORE_LOG_ERROR("No RendererAPI selected");
            MONADO_ASSERT(false, "Error, please choose a Renderer API");
            return nullptr;
        }
        default: break;
        }

        return nullptr;
    }

    std::shared_ptr<TextureCube> TextureCube::Create(const std::vector<std::string> &facesPath) {
        switch (RendererAPI::GetAPIType()) {
        case RendererAPI::APIType::OpenGL: return std::make_shared<OpenGLTextureCube>(facesPath);
        case RendererAPI::APIType::None: {
            CORE_LOG_ERROR("No RendererAPI selected");
            MONADO_ASSERT(false, "Error, please choose a Renderer API");
            return nullptr;
        }
        default: break;
        }

        return nullptr;
    }
} // namespace Monado
