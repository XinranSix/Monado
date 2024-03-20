#pragma once

#include "monado/renderer/shader.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string &filepath);

        virtual void Bind() override;

        virtual void UploadUniformBuffer(const UniformBufferBase &uniformBuffer) override;

    private:
        void ReadShaderFromFile(const std::string &filepath);
        void CompileAndUploadShader();
        static GLenum ShaderTypeFromString(const std::string &type);

        void UploadUniformFloat(const std::string &name, float value);
        void UploadUniformFloat2(const std::string &name, float *values);
        void UploadUniformFloat3(const std::string &name, float *values);
        void UploadUniformFloat4(const std::string &name, const glm::vec4 &values);

    private:
        RendererID m_RendererID;

        std::string m_ShaderSource;
    };

} // namespace Monado
