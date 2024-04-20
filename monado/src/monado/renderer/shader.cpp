
#include "monado/renderer/shader.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/platform/opengl/openGLShader.h"
#include "monado/platform/vulkan/vulkanShader.h"

namespace Monado {

    std::vector<Ref<Shader>> Shader::s_AllShaders;

    Ref<Shader> Shader::Create(const std::string &filepath, bool forceCompile) {
        Ref<Shader> result = nullptr;

        switch (RendererAPI::Current()) {
        case RendererAPIType::None: return nullptr;
        case RendererAPIType::OpenGL: result = Ref<OpenGLShader>::Create(filepath, forceCompile); break;
        case RendererAPIType::Vulkan: result = Ref<VulkanShader>::Create(filepath, forceCompile); break;
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

    void ShaderLibrary::Add(const Hazel::Ref<Shader> &shader) {
        auto &name = shader->GetName();
        MND_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string &path, bool forceCompile) {
        auto shader = Shader::Create(path, forceCompile);
        auto &name = shader->GetName();
        MND_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Load(const std::string &name, const std::string &path) {
        MND_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
        m_Shaders[name] = Shader::Create(path);
    }

    const Ref<Shader> &ShaderLibrary::Get(const std::string &name) const {
        MND_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
        return m_Shaders.at(name);
    }

    ShaderUniform::ShaderUniform(const std::string &name, ShaderUniformType type, uint32_t size, uint32_t offset)
        : m_Name(name), m_Type(type), m_Size(size), m_Offset(offset) {}

    const std::string &ShaderUniform::UniformTypeToString(ShaderUniformType type) {
        if (type == ShaderUniformType::Bool) {
            return "Boolean";
        } else if (type == ShaderUniformType::Int) {
            return "Int";
        } else if (type == ShaderUniformType::Float) {
            return "Float";
        }

        return "None";
    }

} // namespace Monado
