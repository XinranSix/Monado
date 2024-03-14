#include "platform/opengl/openglShader.h"
#include "monado/debug/instrumentor.h"
#include "monado/core/base.h"
#include <filesystem>
#include <fstream>

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <glm/gtc/type_ptr.hpp>

// vulkanµÄ SPIR-V¶«Î÷
// #include <shaderc/shaderc.hpp>
// #include <spirv_cross/spirv_cross.hpp>
// #include <spirv_cross/spirv_glsl.hpp>

#include "monado/core/timer.h"

namespace Monado {
    namespace Utils {
        static GLenum ShaderTypeFromString(const std::string &type) {
            if (type == "vertex") {
                return GL_VERTEX_SHADER;
            }
            if (type == "fragment" || type == "pixel") {
                return GL_FRAGMENT_SHADER;
            }
            MND_CORE_ASSERT(false, "Unknown shader type!F");
            return 0;
        }
        // static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage) {
        //     switch (stage) {
        //     case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
        //     case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
        //     }
        //     MND_CORE_ASSERT(false, "²»ÖªµÀµÄshaderÀàÐÍ");
        //     return (shaderc_shader_kind)0;
        // }
        static const char *GLShaderStageToString(GLenum stage) {
            switch (stage) {
            case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
            }
            MND_CORE_ASSERT(false, "Unknown shader stage!");
            return nullptr;
        }
        static const char *GetCacheDirectory() {
            // TODO: 根据需求修改
            return "assets/cache/shader/opengl";
        }
        static void CreateCacheDirectoryIfNeeded() {
            std::string cacheDirectory = GetCacheDirectory();
            if (!std::filesystem::exists(cacheDirectory)) {
                std::filesystem::create_directories(cacheDirectory); // µÝ¹é´´½¨Ä¿Â¼
            }
        }
        static const char *GLShaderStageCachedOpenGLFileExtension(uint32_t stage) {
            switch (stage) {
            case GL_VERTEX_SHADER: return ".cached_opengl.vert";
            case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
            }
            MND_CORE_ASSERT(false, "²»ÖªµÀµÄshaderÀàÐÍ");
            return "";
        }
    } // namespace Utils

    OpenGLShader::OpenGLShader(const std::string &filepath) : m_FilePath { filepath } {
        MND_PROFILE_FUNCTION();

        // ´´½¨Ä¿Â¼
        // Utils::CreateCacheDirectoryIfNeeded();

        std::string source = ReadFile(filepath);
        MND_CORE_ASSERT(source.size(), "Source is empty!");
        auto shaderSources = PreProcess(source);
        Compile(shaderSources);
        {
            Timer timer;
            //  CompileOrGetVulkanBinaries(shaderSources);
            // CompileOrGetOpenGLBinaries();
            // CreateProgram();
            MND_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
        }
        /*
                asserts\shaders\Texture.glsl
                asserts/shaders/Texture.glsl
                Texture.glsl
        */
        auto lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filepath.rfind('.');
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
        : m_Name(name) {
        MND_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;
        shaderSources[GL_VERTEX_SHADER] = vertexSrc;
        shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(shaderSources);

        //     CompileOrGetVulkanBinaries(shaderSources);
        //   CompileOrGetOpenGLBinaries();
        // CreateProgram();
    }

    OpenGLShader::~OpenGLShader() {
        MND_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);
    }
    std::string OpenGLShader::ReadFile(const std::string &filepath) {
        MND_PROFILE_FUNCTION();

        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary); // ¶þ½øÖÆ¶ÁÈ¡£¿ÎªÊ²Ã´»¹ÊÇ±£³Ö×Ö·û´®µÄÐÎÊ½£¿
        if (in) {
            in.seekg(0, std::ios::end); // ½«Ö¸Õë·ÅÔÚ×îºóÃæ
            size_t size = in.tellg();
            if (size != -1) {
                result.resize(in.tellg());          // ³õÊ¼»¯stringµÄ´óÐ¡, in.tellg()·µ»ØÎ»ÖÃ
                in.seekg(0, std::ios::beg);         // inÖ¸»ØÍ·²¿
                in.read(&result[0], result.size()); // in¶ÁÈë·ÅÔÚresultÖ¸ÏòµÄÄÚ´æÖÐ
            } else {
                MND_CORE_ERROR("Could not read from file '{0}'", filepath);
            }
        } else {
            MND_CORE_ERROR("Could open file '{0}'", filepath);
        }
        return result;
    }
    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source) {
        MND_PROFILE_FUNCTION();

        std::unordered_map<GLenum, std::string> shaderSources;

        std::string typeToken = "#type";
        size_t typeTokenLen = typeToken.size();
        size_t findCurPos = source.find(typeToken, 0);
        size_t findNextPos = findCurPos;
        while (findNextPos != std::string::npos) {
            size_t curlineEndPos = source.find_first_of("\r\n", findCurPos); /// r/nÐ´´íÎª/r/n
            MND_CORE_ASSERT((curlineEndPos != std::string::npos), "Error");
            size_t begin = findCurPos + typeTokenLen + 1;

            std::string type = source.substr(begin, curlineEndPos - begin); // »ñÈ¡µ½ÊÇvertex»¹ÊÇfragment
            MND_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Error");

            size_t nextLinePos = source.find_first_not_of("\r\n", curlineEndPos);
            findNextPos = source.find(typeToken, nextLinePos);
            // »ñÈ¡µ½¾ßÌåµÄshader´úÂë
            shaderSources[Utils::ShaderTypeFromString(type)] = source.substr(
                nextLinePos, findNextPos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

            findCurPos = findNextPos;
        }
        return shaderSources;
        /*
                ÓÃfind£¬¶ø²»ÊÇfind_firtst_of£¬ÒòÎª
                find·µ»ØÍêÈ«Æ¥ÅäµÄ×Ö·û´®µÄµÄÎ»ÖÃ£»
                find_first_of·µ»Ø±»²éÆ¥Åä×Ö·û´®ÖÐÄ³¸ö×Ö·ûµÄµÚÒ»´Î³öÏÖÎ»ÖÃ¡£

                std::string::nposÊÇÒ»¸ö·Ç³£´óµÄÊý
                source.substr(0, source.size() + 10000)½ØÈ¡µ½´ÓÍ·µ½Ä©Î²£¬²»»á±¨´í
        */
    }
    /*    void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string> &shaderSources) {
           GLuint program = glCreateProgram();

           shaderc::Compiler compiler;
           shaderc::CompileOptions options;
           options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
           const bool optimize = true;
           if (optimize) {
               // ÓÅ»¯£ºÐÔÄÜÓÅÏÈ
               options.SetOptimizationLevel(shaderc_optimization_level_performance);
           }
           // Éú³É¶þ½øÖÆµÄ»º´æÄ¿Â¼
           std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

           auto &shaderData = m_VulkanSPIRV;
           shaderData.clear();
           for (auto &&[stage, source] : shaderSources) {
               std::filesystem::path shaderFilePath = m_FilePath;
               std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() +
                                                                    Utils::GLShaderStageCachedOpenGLFileExtension(stage));

               std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
               // »º´æÊÇ·ñ´æÔÚ
               if (in.is_open()) { // ´æÔÚ´ò¿ª¼ÓÔØ
                   in.seekg(0, std::ios::end);
                   auto size = in.tellg();
                   in.seekg(0, std::ios::beg);

                   auto &data = shaderData[stage]; // ?
                   data.resize(size / sizeof(uint32_t));
                   in.read((char *)data.data(), size);
               } else {
                   // ½«VulkanµÄglsl±àÒë³ÉSPIR-V¶þ½øÖÆÎÄ¼þ
                   shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                       source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
                   if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                       HZ_CORE_ERROR(module.GetErrorMessage());
                       HZ_CORE_ASSERT(false);
                   }

                   shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

                   std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                   if (out.is_open()) {
                       auto &data = shaderData[stage];
                       out.write((char *)data.data(), data.size() * sizeof(uint32_t));
                       out.flush();
                       out.close();
                   }
               }
           }
           for (auto &&[stage, data] : shaderData)
               Reflect(stage, data);
       }
       void OpenGLShader::CompileOrGetOpenGLBinaries() {
           auto &shaderData = m_OpenGLSPIRV;

           shaderc::Compiler compiler;
           shaderc::CompileOptions options;
           options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
           const bool optimize = true;
           if (optimize) {
               // ÓÅ»¯£ºÐÔÄÜÓÅÏÈ
               options.SetOptimizationLevel(shaderc_optimization_level_performance);
           }
           // Éú³É¶þ½øÖÆµÄ»º´æÄ¿Â¼
           std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

           shaderData.clear();
           m_OpenGLSourceCode.clear();
           for (auto &&[stage, spirv] : m_VulkanSPIRV) {
               std::filesystem::path shaderFilePath = m_FilePath;
               std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() +
                                                                    Utils::GLShaderStageCachedOpenGLFileExtension(stage));

               std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
               // »º´æÊÇ·ñ´æÔÚ
               if (in.is_open()) { // ´æÔÚ´ò¿ª¼ÓÔØ
                   in.seekg(0, std::ios::end);
                   auto size = in.tellg();
                   in.seekg(0, std::ios::beg);

                   auto &data = shaderData[stage]; // ?
                   data.resize(size / sizeof(uint32_t));
                   in.read((char *)data.data(), size);
               } else {
                   // ½«VulkanµÄglslµÄSPIR-V¶þ½øÖÆÎÄ¼þ×ª»»ÎªOpenGLµÄglslÔ´ÎÄ¼þ×Ö·û´®
                   spirv_cross::CompilerGLSL glslCompiler(spirv);
                   m_OpenGLSourceCode[stage] = glslCompiler.compile();
                   auto &source = m_OpenGLSourceCode[stage];
                   // ÔÙ½«OpenGLµÄglslÔ´ÎÄ¼þ×Ö·û´®×ª»»ÎªSPIR-V¶þ½øÖÆÎÄ¼þ£¬²¢ÇÒ±£´æÔÚ±¾µØ×÷Îª»º´æ
                   shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                       source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
                   if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                       HZ_CORE_ERROR(module.GetErrorMessage());
                       HZ_CORE_ASSERT(false);
                   }

                   shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

                   std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                   if (out.is_open()) {
                       auto &data = shaderData[stage];
                       out.write((char *)data.data(), data.size() * sizeof(uint32_t));
                       out.flush();
                       out.close();
                   }
               }
           }
       } */
    void OpenGLShader::CreateProgram() {
        GLuint program = glCreateProgram();

        std::vector<GLuint> shaderIDs;
        for (auto &&[stage, spirv] : m_OpenGLSPIRV) {
            GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
            glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
            glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
            glAttachShader(program, shaderID);
        }
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            MND_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            for (auto id : shaderIDs) {
                glDeleteShader(id);
            }
            // Use the infoLog as you see fit.
            // In this simple program, we'll just leave
            MND_CORE_ERROR("{0} ", infoLog.data());
            MND_CORE_ASSERT(false, "shader link failure!");
        }

        // Always detach shaders after a successful link.
        for (auto id : shaderIDs) {
            glDetachShader(program, id);
            glDeleteShader(id);
        }
        m_RendererID = program;
    }
    /*   void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t> &shaderData) {
          spirv_cross::Compiler compiler(shaderData);
          spirv_cross::ShaderResources resources = compiler.get_shader_resources();

          HZ_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
          HZ_CORE_TRACE("		{0} uniform buffers", resources.uniform_buffers.size());
          HZ_CORE_TRACE("		{0} resources", resources.sampled_images.size());

          HZ_CORE_TRACE("Uniform buffers:");
          for (const auto &resource : resources.uniform_buffers) {
              const auto &bufferType = compiler.get_type(resource.base_type_id);
              uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
              uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
              int memberCount = bufferType.member_types.size();

              HZ_CORE_TRACE("		{0}", resource.name);
              HZ_CORE_TRACE("		size = {0}", bufferSize);
              HZ_CORE_TRACE("		Binding = {0}", binding);
              HZ_CORE_TRACE("		Members = {0}", memberCount);
          }
      }
   */
    void OpenGLShader::Bind() const {
        //  MND_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }
    void OpenGLShader::Unbind() const {
        //  MND_PROFILE_FUNCTION();

        glUseProgram(0);
    }
    void OpenGLShader::SetInt(const std::string &name, int value) {
        //  MND_PROFILE_FUNCTION();

        UploadUniformInt(name, value);
    }
    void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count) {
        UploadUniformIntArray(name, values, count);
    }
    void OpenGLShader::SetFloat(const std::string &name, float value) {
        //  MND_PROFILE_FUNCTION();

        UploadUniformFloat(name, value);
    }
    void OpenGLShader::SetFloat2(const std::string &name, const glm::vec2 &value) {
        //  MND_PROFILE_FUNCTION();

        UploadUniformFloat2(name, value);
    }
    void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value) {
        //  MND_PROFILE_FUNCTION();

        UploadUniformFloat3(name, value);
    }
    void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value) {
        //  MND_PROFILE_FUNCTION();

        UploadUniformFloat4(name, value);
    }
    void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value) {
        //  MND_PROFILE_FUNCTION();
        UploadUniformMat4(name, value);
    }
    void OpenGLShader::UploadUniformInt(const std::string &name, int value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }
    void OpenGLShader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }
    void OpenGLShader::UploadUniformFloat(const std::string &name, float value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }
    void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }
    void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }
    void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
    void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::mat3 &matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    // namespace Monado

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string> &shaderSources) {
        // HZ_PROFILE_FUNCTION();

        GLuint program = glCreateProgram();
        /*
                std::vector<GLenum> glShaderIDs(shaderSources.size());
                glShaderIDs.reserve(2);
        */
        std::array<GLenum, 2> glShaderIDs;
        int glShaderIDIndex = 0;
        for (auto &kv : shaderSources) {
            GLenum type = kv.first;
            const std::string &source = kv.second;

            // Create an empty vertex shader handle
            GLuint shader = glCreateShader(type);

            // Send the vertex shader source code to GL
            // Note that std::string's .c_str is NULL character terminated.
            const GLchar *sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);

            // Compile the vertex shader
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

                // In this simple program, we'll just leave
                MND_CORE_ERROR("{0} ", infoLog.data());
                MND_CORE_ASSERT(false, "shader ±àÒëÊ§°Ü!");

                break;
            }

            // Attach our shaders to our program
            glAttachShader(program, shader);

            // glShaderIDs.push_back(shader);
            glShaderIDs[glShaderIDIndex++] = shader;
        }
        //
        m_RendererID = program;

        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            for (auto id : glShaderIDs) {
                glDeleteShader(id);
            }
            // Use the infoLog as you see fit.
            // In this simple program, we'll just leave
            MND_CORE_ERROR("{0} ", infoLog.data());
            MND_CORE_ASSERT(false, "shader link failure!");
            return;
        }

        // Always detach shaders after a successful link.
        for (auto id : glShaderIDs) {
            glDetachShader(program, id);
        }
    }
} // namespace Monado
  // namespace Monado
