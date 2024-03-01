// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on
#include "platform/opengl/openGLShader.h"
// #include "monado/Utils/Utils.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/renderCommand.h"
#include "monado/core/log.h"
#include "monado/core/core.h"

// #include <shaderc/shaderc.hpp>
#include <fstream>
#include <iostream>

// #include <spirv_cross/spirv_cross.hpp>
// #include <spirv_cross/spirv_glsl.hpp>
// #include <Renderer/RenderCommandRegister.h>

namespace Monado {
    static unsigned int StringToGLShaderType(const std::string &name) {
        if (name == "vertex")
            return GL_VERTEX_SHADER;
        if (name == "fragment")
            return GL_FRAGMENT_SHADER;
        if (name == "geometry")
            return GL_GEOMETRY_SHADER;
        if (name == "compute")
            return GL_COMPUTE_SHADER;

        MONADO_CORE_ASSERT(0, "Unsupported shader type: " + name);
        return -1;
    }

    OpenGLShader::OpenGLShader(const std::string &path) {
        // 1. 读取文件内容, 存到result这个string里
        std::ifstream in(path);
        // g是get的意思, seekg会把读取位置放到文件末尾
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        // 创建一个含有size个' '字符的字符串
        std::string result(size, ' ');
        in.seekg(0);
        in.read(&result[0], size);

        // 2. 分隔这个result, 获取多个Shader(对C++字符串不熟悉, 这段代码写的比较混乱)
        // TODO: optimization
        std::vector<GLuint> shaders;
        while (result.find("#type ", 0) != std::string::npos) {
            // 先获取#type后面的shader字符串类型
            // 注意, 转义字符是\r\n, windows上代表换行, \r是光标移到行首, \n是光标跳到下一行
            // sizeof("\r\n") = 3, sizeof("/r/n") = 5
            size_t p = result.find_first_of("#type ", 0);
            int c = sizeof("\r\n") - 1;
            size_t pEnd = result.find_first_of("\r\n", p) + 2; // /r/n是四个字符
            size_t typeLength = pEnd - 2 - p - (sizeof("#type ") - 1);
            std::string type = result.substr(p + (sizeof("#type ") - 1), typeLength);
            unsigned int typeID = StringToGLShaderType(type);

            // 然后移除#type这一行以及这一行之前的空白行(如果有的话)
            size = size - pEnd;
            result = result.substr(pEnd, size);

            // 找到下一个Shader的开头
            size_t newP = result.find("#type ", 0);

            std::string shaderSrc;
            if (newP == std::string::npos)
                shaderSrc = result;
            else {
                shaderSrc = result.substr(0, newP - 1);
                size_t newSize = result.size() - newP;
                result = result.substr(newP, newSize);
            }

            GLuint shader = glCreateShader(typeID);

            // Send the shader source code to GL
            // Note that std::string's .c_str is NULL character terminated.
            const GLchar *source = (const GLchar *)shaderSrc.c_str();
            glShaderSource(shader, 1, &source, 0);

            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                // We don't need the shader anymore.
                glDeleteShader(shader);

                // Use the infoLog as you see fit.
                CORE_LOG_ERROR("Compile {0} Shader Failed!:{1}", type, &infoLog[0]);
                MONADO_ASSERT(false, "Compile {0} Shader Error Stopped Debugging!", type);
            }

            shaders.push_back(shader);
        }

        // 3. 创建ShaderProgram
        m_RendererID = glCreateProgram();

        for (GLuint item : shaders) {
            glAttachShader(m_RendererID, item);
        }

        // Link our program
        glLinkProgram(m_RendererID);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_RendererID);

            // Don't leak shaders either.
            for (GLuint item : shaders) {
                glDeleteShader(item);
            }

            // Use the infoLog as you see fit.
            CORE_LOG_ERROR("Link Shaders Failed!:{0}", infoLog[0]); // TODO这里应该是错的
            MONADO_ASSERT(false, "Link  Shaders Error Stopped Debugging!");

            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        for (GLuint item : shaders) {
            glDetachShader(m_RendererID, item);
        }
    }

    OpenGLShader::OpenGLShader(const std::string &vertSource, const std::string &fragSource) {
        // Create an empty vertex shader handle
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar *source = (const GLchar *)vertSource.c_str();
        glShaderSource(vertexShader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(vertexShader);

        GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(vertexShader);

            // Use the infoLog as you see fit.
            CORE_LOG_ERROR("Compile Vertex Shader Failed!:{0}", &infoLog[0]);
            MONADO_ASSERT(false, "Compile Vertex Shader Error Stopped Debugging!");

            // In this simple program, we'll just leave
            return;
        }

        // Create an empty fragment shader handle
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const GLchar *)fragSource.c_str();
        glShaderSource(fragmentShader, 1, &source, 0);

        // Compile the fragment shader
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragmentShader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertexShader);

            // Use the infoLog as you see fit.
            CORE_LOG_ERROR("Compile Fragment Shader Failed!:{0}", &infoLog[0]);
            MONADO_ASSERT(false, "Compile Fragment Shader Error Stopped Debugging!");

            // In this simple program, we'll just leave
            return;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        m_RendererID = glCreateProgram();

        // Attach our shaders to our program
        glAttachShader(m_RendererID, vertexShader);
        glAttachShader(m_RendererID, fragmentShader);

        // Link our program
        glLinkProgram(m_RendererID);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(m_RendererID);
            // Don't leak shaders either.
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            // Use the infoLog as you see fit.
            CORE_LOG_ERROR("Link Shaders Failed!:{0}", infoLog[0]); // TODO这里应该是错的
            MONADO_ASSERT(false, "Link  Shaders Error Stopped Debugging!");

            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(m_RendererID, vertexShader);
        glDetachShader(m_RendererID, fragmentShader);
    }

    OpenGLShader::~OpenGLShader() { glDeleteProgram(m_RendererID); }

    void Monado::OpenGLShader::Bind() { glUseProgram(m_RendererID); }

    void OpenGLShader::Unbind() { glUseProgram(0); }

    void OpenGLShader::UploadUniformMat4(const std::string &uniformName, const glm::mat4 &matrix) {
        // glm::value_ptr返回的类型是GLfloat*, 应该是个数组
        glUniformMatrix4fv(glGetUniformLocation(m_RendererID, uniformName.c_str()), 1, GL_FALSE,
                           glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformVec4(const std::string &uniformName, const glm::vec4 &vec4) {
        glUniform4fv(glGetUniformLocation(m_RendererID, uniformName.c_str()), 1, glm::value_ptr(vec4));
    }

    void OpenGLShader::UploadUniformI1(const std::string &uniformName, int id) {
        glUniform1i(glGetUniformLocation(m_RendererID, uniformName.c_str()), id); // set it manually
    }

    void OpenGLShader::UploadUniformF1(const std::string &uniformName, float number) {
        glUniform1f(glGetUniformLocation(m_RendererID, uniformName.c_str()), number); // set it manually
    }

    void OpenGLShader::UploadUniformIntArr(const std::string &uniformName, int count, int *values) {
        glUniform1iv(glGetUniformLocation(m_RendererID, uniformName.c_str()), count, values);
    }
} // namespace Monado
