
#include "monado/renderer/shader.h"

#include "monado/platform/opengl/openGLShader.h"

namespace Monado {

    Shader *Shader::Create(const std::string &filepath) {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: return new OpenGLShader(filepath);
        }
        return nullptr;
    }

} // namespace Monado
