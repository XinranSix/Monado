#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <memory>

namespace Monado {
    // 这里的Shader对象其实是一个ShaderProgram, 里面包含了vert和frag
    class Shader {
    public:
        virtual ~Shader() {} // 未来会有OpenGLShader、DirectXShader，所以未来虚函数可能是virtual的
        virtual void Bind() = 0;
        virtual void
        Unbind() = 0; // 其实这个函数一般运行时不会用到，一般用于Debug，因为使用的时候会直接Bind新的，就会自动Unbind这个
        virtual void UploadUniformMat4(const std::string &uniformName, const glm::mat4 &matrix) = 0;
        virtual void UploadUniformVec4(const std::string &uniformName, const glm::vec4 &vec4) = 0;
        virtual void UploadUniformI1(const std::string &uniformName, int id) = 0;
        virtual void UploadUniformF1(const std::string &uniformName, float number) = 0;
        virtual void UploadUniformIntArr(const std::string &uniformName, int count, int *number) = 0;

        static std::shared_ptr<Shader> Create(const std::string &path);
        static std::shared_ptr<Shader> Create(const std::string &vertSource, const std::string &fragSource);

    protected:
        unsigned int m_RendererID; // 其实就是program对应的id，program里存放了一套渲染管线的shader
    };

    enum class ShaderType { InValid = -1, Vertex, Fragement, Compute, Geometry };
} // namespace Monado
