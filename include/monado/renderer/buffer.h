#pragma once

#include <cstdint>
#include <string>
#include "monado/core/base.h"

namespace Monado {

    enum class ShaderDataType { None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool };

    static uint32_t ShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
        case Monado::ShaderDataType::Float: return 4;
        case Monado::ShaderDataType::Float2: return 4 * 2;
        case Monado::ShaderDataType::Float3: return 4 * 3;
        case Monado::ShaderDataType::Float4: return 4 * 4;
        case Monado::ShaderDataType::Mat3: return 4 * 3 * 3;
        case Monado::ShaderDataType::Mat4: return 4 * 4 * 4;
        case Monado::ShaderDataType::Int: return 4;
        case Monado::ShaderDataType::Int2: return 4 * 2;
        case Monado::ShaderDataType::Int3: return 4 * 3;
        case Monado::ShaderDataType::Int4: return 4 * 4;
        case Monado::ShaderDataType::Bool: return 1;
        }
        MND_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }
    struct BufferElement {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        uint32_t Offset;
        bool Normalized;

        BufferElement(ShaderDataType type, const std::string &name, bool Normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(Normalized) {}

        uint32_t GetComponentCount() const {
            switch (Type) {
            case Monado::ShaderDataType::Float: return 1;
            case Monado::ShaderDataType::Float2: return 2;
            case Monado::ShaderDataType::Float3: return 3;
            case Monado::ShaderDataType::Float4: return 4;
            case Monado::ShaderDataType::Mat3: return 3 * 3;
            case Monado::ShaderDataType::Mat4: return 4 * 4;
            case Monado::ShaderDataType::Int: return 1;
            case Monado::ShaderDataType::Int2: return 2;
            case Monado::ShaderDataType::Int3: return 3;
            case Monado::ShaderDataType::Int4: return 4;
            case Monado::ShaderDataType::Bool: return 1;
            }
            MND_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class BufferLayout {
    public:
        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement> &elements) : m_Elements(elements) {
            CalculateOffsetAndStride();
        }
        inline uint32_t GetStride() const { return m_Stride; }
        inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }
        // ��������Ϊ��ѭ��
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

    private:
        void CalculateOffsetAndStride() {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto &element : m_Elements) {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void *data, uint32_t size) = 0;

        virtual const BufferLayout &GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout &layout) = 0;

        static Ref<VertexBuffer> Create(uint32_t size);
        static Ref<VertexBuffer> Create(float *vertices, uint32_t size);
    };

    class IndexBuffer {
    public:
        virtual ~IndexBuffer() {}
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetCount() const = 0;

        static Ref<IndexBuffer> Create(uint32_t *indices, uint32_t count);
    };
} // namespace Monado
