#include "monado/renderer/shader.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/renderer/renderer.h"
#include "platform/openGL/openGLShader.h"
#include <format>

namespace Monado {
    Ref<Shader> Shader::Create(const std::string &filepath) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
        }
        MND_CORE_ASSERT(false, "UnKnown RendererAPI!");
        return nullptr;
    }
    Ref<Shader> Shader::Create(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
        }
        MND_CORE_ASSERT(false, "0x1908UnKnown RendererAPI!");
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////
    // ShaderLibrary//////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////
    void ShaderLibrary::Add(const Ref<Shader> &shader) {
        auto &name = shader->GetName();
        Add(name, shader);
    }
    void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &shader) {
        MND_CORE_ASSERT(!Exists(name), std::format("Shader [{}] is already existed!", name).c_str());
        m_Shaders[name] = shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string &filepath) {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::string &filepath) {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Get(const std::string &name) {
        MND_CORE_ASSERT(Exists(name), std::format("Shader [{}] dose not exists", name).c_str());
        return m_Shaders[name];
    }
    bool ShaderLibrary::Exists(const std::string &name) const { return m_Shaders.find(name) != m_Shaders.end(); }
} // namespace Monado
