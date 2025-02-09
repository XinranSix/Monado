#pragma once

#include "monado/core/base.h"

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Monado {
    class Shader {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetInt(const std::string &name, int value) = 0;
        virtual void SetIntArray(const std::string &name, int *values, uint32_t count) = 0;
        virtual void SetFloat(const std::string &name, float value) = 0;
        virtual void SetFloat2(const std::string &name, const glm::vec2 &value) = 0;
        virtual void SetFloat3(const std::string &name, const glm::vec3 &value) = 0;
        virtual void SetFloat4(const std::string &name, const glm::vec4 &value) = 0;
        virtual void SetMat4(const std::string &name, const glm::mat4 &value) = 0;

        virtual const std::string &GetName() const = 0;

        static Monado::Ref<Shader> Create(const std::string &filepath);
        static Monado::Ref<Shader> Create(const std::string &name, const std::string &vertexSrc,
                                          const std::string &fragmentSrc);
    };
    
    class ShaderLibrary {
    public:
        void Add(const std::string &name, const Monado::Ref<Shader> &shader);
        void Add(const Monado::Ref<Shader> &shader);
        Monado::Ref<Shader> Load(const std::string &filepath);
        Monado::Ref<Shader> Load(const std::string &name, const std::string &filepath);

        Monado::Ref<Shader> Get(const std::string &name);

        bool Exists(const std::string &name) const;

    private:
        std::unordered_map<std::string, Monado::Ref<Shader>> m_Shaders;
    };
} // namespace Moando
