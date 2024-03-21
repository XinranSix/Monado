
#include "monado/renderer/shader.h"
#include "monado/renderer/renderer.h"
#include "monado/platform/opengl/openGLShader.h"

namespace Monado {

    std::vector<Ref<Shader>> Shader::s_AllShaders;

    Ref<Shader> Shader::Create(const std::string &filepath) {
        Ref<Shader> result = nullptr;

        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: result = std::make_shared<OpenGLShader>(filepath);
        }
        s_AllShaders.push_back(result);
        return result;
    }

    Ref<Shader> Shader::CreateFromString(const std::string &source) {
        Ref<Shader> result = nullptr;

        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: result = OpenGLShader::CreateFromString(source);
        }
        s_AllShaders.push_back(result);
        return result;
    }

    ShaderLibrary::ShaderLibrary() {}

    ShaderLibrary::~ShaderLibrary() {}

    void ShaderLibrary::Add(const Monado::Ref<Shader> &shader) {
        auto &name = shader->GetName();
        MND_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string &path) {
        auto shader = Ref<Shader>(Shader::Create(path));
        auto &name = shader->GetName();
        MND_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string &name, const std::string &path) {
        MND_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = Ref<Shader>(Shader::Create(path));
    }

    Ref<Shader> &ShaderLibrary::Get(const std::string &name) {
        MND_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
        return m_Shaders[name];
    }

} // namespace Monado
