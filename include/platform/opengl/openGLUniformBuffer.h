#pragma once

#include "monado/renderer/uniformBuffer.h"

namespace Monado {
    class OpenGLUniformBuffer : public UniformBuffer {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        ~OpenGLUniformBuffer() override;
        void SetData(const void *data, uint32_t size, uint32_t offset = 0) override;

    private:
        uint32_t m_BufferID = 0;
    };
} // namespace Monado
