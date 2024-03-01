#include "monado/renderer/shader.h"
#include "monado/renderer/renderer.h"
#include "platform/openGL/OpenGLShader.h"

namespace Monado {
    std::shared_ptr<Shader> Shader::Create(const std::string &path) {
        RendererAPI::APIType type = Renderer::GetAPI();
        switch (type) {
        case RendererAPI::APIType::OpenGL: return std::make_shared<OpenGLShader>(path);
        case RendererAPI::APIType::None: return nullptr;
        default: break;
        }

        return nullptr;
    }

    std::shared_ptr<Shader> Shader::Create(const std::string &vertSource, const std::string &fragSource) {
        RendererAPI::APIType type = Renderer::GetAPI();
        switch (type) {
        case RendererAPI::APIType::OpenGL: return std::make_shared<OpenGLShader>(vertSource, fragSource);
        case RendererAPI::APIType::None: return nullptr;
        default: break;
        }

        return nullptr;
    }
} // namespace Monado
